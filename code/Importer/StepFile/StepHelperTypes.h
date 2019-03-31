#pragma once

namespace Assimp {
    namespace STEP {
        class Object;
        class LazyObject;
        class DB;
    }

    namespace StepFile {

        struct StepMesh {

        };

        struct ConversionData {
            const STEP::DB &db;
            aiScene* out;

            IfcMatrix4 wcs;
            std::vector<aiMesh*> meshes;
            std::vector<aiMaterial*> materials;

        };

    }
}
