#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_Hypothesis.hxx>
#include <NETGENPlugin_SimpleHypothesis_3D.hxx>
#include <NETGENPlugin_NETGEN_2D3D.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_Regular_1D.hxx>

TEST_CASE("Mesh an edge of a box.", "[StdMeshers][LocalLength]") {

	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	TopExp_Explorer exp = TopExp_Explorer(box, TopAbs_EDGE);
	const TopoDS_Shape& edge = exp.Current();

	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	StdMeshers_LocalLength* hyp1d = new StdMeshers_LocalLength(0, gen);
	hyp1d->SetLength(0.1);
	StdMeshers_Regular_1D* algo1d = new StdMeshers_Regular_1D(1, gen);

	mesh->ShapeToMesh(box);
	mesh->AddHypothesis(edge, 0);
	mesh->AddHypothesis(edge, 1);

	// TODO: Issues with osx tests
	#ifndef __APPLE__
	bool success = gen->Compute(*mesh, box);
	REQUIRE(success == true);

	REQUIRE(mesh->NbNodes() == 107);
	#endif

	delete hyp1d;
	delete algo1d;
	delete mesh;
	delete gen;
}

TEST_CASE("Mesh a box with tetrahedral elements.", "[NETGENPlugin]") {

    TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

    SMESH_Gen* gen = new SMESH_Gen();
    SMESH_Mesh* mesh = gen->CreateMesh(true);

    NETGENPlugin_SimpleHypothesis_3D* hyp = new NETGENPlugin_SimpleHypothesis_3D(0, gen);
    hyp->SetLocalLength(1.0);

    NETGENPlugin_NETGEN_2D3D* algo = new NETGENPlugin_NETGEN_2D3D(1, gen);

    mesh->ShapeToMesh(box);
    mesh->AddHypothesis(box, 0);
    mesh->AddHypothesis(box, 1);

    // TODO: Issues with osx tests
	#ifndef __APPLE__
    bool success = gen->Compute(*mesh, box);
    REQUIRE(success == true);

    REQUIRE(mesh->NbTetras() == 4671);
    REQUIRE(mesh->NbNodes() == 1172);
    #endif

    delete hyp;
    delete algo;
    delete mesh;
    delete gen;
}

TEST_CASE("Mesh a box with tetrahedral elements and a local edge length.", "[NETGENPlugin]") {

	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	TopExp_Explorer exp = TopExp_Explorer(box, TopAbs_EDGE);
	const TopoDS_Shape& edge = exp.Current();

	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	NETGENPlugin_SimpleHypothesis_3D* hyp3d = new NETGENPlugin_SimpleHypothesis_3D(0, gen);
	hyp3d->SetLocalLength(1.0);
	NETGENPlugin_NETGEN_2D3D* algo3d = new NETGENPlugin_NETGEN_2D3D(1, gen);

	StdMeshers_LocalLength* hyp1d = new StdMeshers_LocalLength(2, gen);
	hyp1d->SetLength(0.1);
	StdMeshers_Regular_1D* algo1d = new StdMeshers_Regular_1D(3, gen);

	mesh->ShapeToMesh(box);
	mesh->AddHypothesis(box, 0);
	mesh->AddHypothesis(box, 1);
	mesh->AddHypothesis(edge, 2);
	mesh->AddHypothesis(edge, 3);

	// TODO: Issues with osx tests
	#ifndef __APPLE__
	bool success = gen->Compute(*mesh, box);
	REQUIRE(success == true);

	REQUIRE(mesh->NbTetras() == 34068);
	REQUIRE(mesh->NbNodes() == 6665);
	#endif

	delete hyp3d;
	delete algo3d;
	delete hyp1d;
	delete algo1d;
	delete mesh;
	delete gen;
}
