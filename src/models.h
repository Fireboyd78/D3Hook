#pragma once
#include "driv3r.h"

struct alignas(4) MeshDefinition
{
    D3DPRIMITIVETYPE PrimitiveType;

    INT BaseVertexIndex;
    UINT MinIndex;
    UINT NumVertices;
    UINT StartIndex;
    UINT PrimitiveCount;

    bool IsNonIndexedPrimitive;

    short field_1C;
    short field_1E;

    long Reserved1;
    long Reserved2;
    long Reserved3;
    long Reserved4;

    short MaterialId;
    short SourceUID;

    LPVOID Material;
};

struct MeshGroup
{
    DWORD DataOffset;

    MeshDefinition **Meshes;

    MAv4 TransformX;
    MAv4 TransformY;
    MAv4 TransformZ;

    MAv4 Axis_Thing;

    short Count;
    short Unknown;

    DWORD Reserved1;
    DWORD Reserved2;
    DWORD Reserved3;
};

struct LodEntry {
    DWORD DataOffset;

    MeshGroup *MeshGroups;
    INT nMeshGroups;

    DWORD Reserved1;
    DWORD Reserved2;

    INT Type;

    DWORD Reserved3;
    DWORD Reserved4;
};

struct ModelContainer {
    UINT UID;
    UINT Handle;
    UINT Reserved1[4];

    short Unknown1;
    short VertexBufferId;

    UINT Unknown2;

    UINT Reserved2;
    UINT Reserved3;

    MAv4 Frustrum[8];

    LodEntry Entries[7];
};