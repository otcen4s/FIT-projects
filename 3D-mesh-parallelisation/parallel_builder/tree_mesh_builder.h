/**
 * @file    tree_mesh_builder.h
 *
 * @author  MATEJ OTCENAS <xotcen01@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    10.12.2021
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
    unsigned octree(const ParametricScalarField &field, unsigned gridSize, const Vec3_t<float> &pos);
    bool isEmpty(unsigned gridSize, const Vec3_t<float> &pos, const ParametricScalarField &field);

    std::vector<Triangle_t> mTriangles;
};

#endif // TREE_MESH_BUILDER_H
