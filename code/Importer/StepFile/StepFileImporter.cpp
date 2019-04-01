/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2019, assimp team



All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

#ifndef ASSIMP_BUILD_NO_STEP_IMPORTER

#include "StepFileImporter.h"
#include "../../Importer/STEPParser/STEPFileReader.h"
#include "StepReaderGen.h"
#include <assimp/importerdesc.h>
#include <assimp/DefaultIOSystem.h>

namespace Assimp {
namespace StepFile {

using namespace STEP;

struct StepFileMesh {
    std::vector<const ::Assimp::StepFile::cartesian_point*> mVertices;
};

struct StepFileData {
    std::vector<StepFileMesh*> mMeshes;
};

static const aiImporterDesc desc = { "StepFile Importer",
                                "",
                                "",
                                "",
                                0,
                                0,
                                0,
                                0,
                                0,
                                "stp" };

StepFileImporter::StepFileImporter()
: BaseImporter() {

}

StepFileImporter::~StepFileImporter() {

}

bool StepFileImporter::CanRead(const std::string& file, IOSystem* pIOHandler, bool checkSig) const {
    const std::string &extension = GetExtension(file);
    if ( extension == "stp" || extension == "step" ) {
        return true;
    } else if ((!extension.length() || checkSig) && pIOHandler) {
        const char* tokens[] = { "ISO-10303-21" };
        const bool found(SearchFileHeaderForToken(pIOHandler, file, tokens, 1));
        return found;
    }

    return false;
}

const aiImporterDesc *StepFileImporter::GetInfo() const {
    return &desc;
}

static const char *mode = "rb";
static const char *StepFileSchema = "CONFIG_CONTROL_DESIGN";

void StepFileImporter::InternReadFile(const std::string &file, aiScene* pScene, IOSystem* pIOHandler) {
    // Read file into memory
    std::shared_ptr<IOStream> fileStream(pIOHandler->Open(file, mode));
    if (!fileStream.get()) {
        throw DeadlyImportError("Failed to open file " + file + ".");
    }

    std::unique_ptr<STEP::DB> db(STEP::ReadFileHeader(fileStream));
    const STEP::HeaderInfo& head = static_cast<const STEP::DB&>(*db).GetHeader();
    if (!head.fileSchema.size() || head.fileSchema != StepFileSchema) {
        DeadlyImportError("Unrecognized file schema: " + head.fileSchema);
    }
    ::Assimp::STEP::EXPRESS::ConversionSchema schema;
    ::Assimp::StepFile::GetSchema( schema );

    static const char* const types_to_track[] = {
        "cartesian_point",
        "face_outer_bound",
        "vertex_point",
        "surface_of_linear_extrusion",
        "plane",
        "line",
        "edge_loop"
    };

    // tell the reader for which types we need to simulate STEPs reverse indices
    static const char* const inverse_indices_to_track[] = {
        "product_definition"
    };

    STEP::ReadFile( *db, schema, types_to_track, inverse_indices_to_track );

    ProcessSpatialStructures( db.get() );
}

void StepFileImporter::ProcessSpatialStructures( STEP::DB *db ) {
    StepFileData *data = new StepFileData;

    StepFileMesh *mesh = new StepFileMesh;
    const STEP::DB::ObjectMapByType& map = db->GetObjectsByType();

    const STEP::DB::ObjectSet *cpRange = &map.find( "cartesian_point" )->second;
    for (const STEP::LazyObject* lz : *cpRange) {
        const ::Assimp::StepFile::cartesian_point *cart_pt = lz->ToPtr<::Assimp::StepFile::cartesian_point>();
        if (nullptr == cart_pt) {
            continue;
        }
        mesh->mVertices.push_back( cart_pt );
    }

    const STEP::DB::ObjectSet *faceRange = &map.find( "face_outer_bound" )->second;
    if (faceRange->size() > 0) {

    }
    for (const STEP::LazyObject* lz : *faceRange) {
        const ::Assimp::StepFile::face_outer_bound *face = lz->ToPtr<::Assimp::StepFile::face_outer_bound>();
        if (nullptr == face) {
            continue;
        }

        // todo!
        //face->bound->
    }

    const STEP::DB::ObjectSet *edgeCurve = &map.find( "edge_curve" )->second;
    for (const STEP::LazyObject* lz : *faceRange) {
        const ::Assimp::StepFile::edge_curve *curEdgeCurve = lz->ToPtr<::Assimp::StepFile::edge_curve>();
        if (nullptr == curEdgeCurve) {
            continue;
        }
        curEdgeCurve->edge_geometry
    }
}
} // Namespace StepFile
} // Namespace Assimp

#endif // ASSIMP_BUILD_NO_STEP_IMPORTER

