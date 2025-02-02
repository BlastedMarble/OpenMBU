//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "customMaterial.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "gfx/gfxDevice.h"
#include "materials/shaderData.h"
#include "gfx/cubemapData.h"
#include "gfx/gfxCubemap.h"
#include "../../game/shaders/shdrConsts.h"
#include "materialPropertyMap.h"


//****************************************************************************
// Custom Material
//****************************************************************************
IMPLEMENT_CONOBJECT(CustomMaterial);

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
CustomMaterial::CustomMaterial()
{
    dMemset(texFilename, 0, sizeof(texFilename));
    fallback = NULL;
    mType = Custom;
    dMemset(pass, 0, sizeof(pass));

    dynamicLightingMaterial = NULL;
    dynamicLightingMaskMaterial = NULL;

    mMaxTex = 0;
    mVersion = 1.1f;
    translucent = false;
    dMemset(mFlags, 0, sizeof(mFlags));
    mCurPass = -1;
    blendOp = Material::Add;
    mShaderData = NULL;
    mShaderDataName = NULL;
    refract = false;
    mCullMode = -1;
}



//--------------------------------------------------------------------------
// Init fields
//--------------------------------------------------------------------------
void CustomMaterial::initPersistFields()
{
    Parent::initPersistFields();

    addField("texture", TypeFilename, Offset(texFilename, CustomMaterial), MAX_TEX_PER_PASS);

    addField("version", TypeF32, Offset(mVersion, CustomMaterial));

    addField("fallback", TypeSimObjectPtr, Offset(fallback, CustomMaterial));
    addField("pass", TypeSimObjectPtr, Offset(pass, CustomMaterial), MAX_PASSES);

    addField("dynamicLightingMaterial", TypeSimObjectPtr, Offset(dynamicLightingMaterial, CustomMaterial));
    addField("dynamicLightingMaskMaterial", TypeSimObjectPtr, Offset(dynamicLightingMaskMaterial, CustomMaterial));

    addField("shader", TypeString, Offset(mShaderDataName, CustomMaterial));
    addField("blendOp", TypeEnum, Offset(blendOp, CustomMaterial), 1, &mBlendOpTable);

    addField("refract", TypeBool, Offset(refract, CustomMaterial));

}

bool CustomMaterial::preloadTextures()
{
    bool found = Parent::preloadTextures();
    for (int i = 0; i < MAX_TEX_PER_PASS; i++)
    {
        found = found && (!texFilename[i] || didFindTexture(texFilename[i]));
    }
    for (int i = 0; i < MAX_PASSES; i++)
    {
        found = found && (!pass[i] || pass[i]->preloadTextures());
    }
    if (fallback != NULL)
        found = found && fallback->preloadTextures();
    found = found && (!mShaderData->DXVertexShaderName || ResourceManager->find(mShaderData->DXVertexShaderName)); // Transfer shaders too lmao (attempt)
    found = found && (!mShaderData->DXVertexShaderName || ResourceManager->find(mShaderData->DXPixelShaderName));

    return found;
}

//--------------------------------------------------------------------------
// On add - verify data settings
//--------------------------------------------------------------------------
bool CustomMaterial::onAdd()
{
    if (Parent::onAdd() == false)
        return false;

    mShaderData = static_cast<ShaderData*>(Sim::findObject(mShaderDataName));

    // Allow translucent objects to be seen behind each other
    //for (S32 i = 0; i < MAX_PASSES; i++)
    //{
    //    CustomMaterial* mat = pass[i];
    //    if (mat && mat->translucent)
    //        subPassTranslucent = true;
    //}

    return true;
}

//--------------------------------------------------------------------------
// On remove
//--------------------------------------------------------------------------
void CustomMaterial::onRemove()
{
    Parent::onRemove();
}

//--------------------------------------------------------------------------
// Map this material to the texture specified in the "mapTo" data variable
//--------------------------------------------------------------------------
void CustomMaterial::mapMaterial()
{
    if (!getName())
    {
        Con::warnf("Unnamed Material!  Could not map to: %s", mapTo);
        return;
    }

    if (!mapTo)
    {
        return;
    }

    MaterialPropertyMap* pMap = MaterialPropertyMap::get();
    if (pMap == NULL)
    {
        Con::errorf(ConsoleLogEntry::General, "Error, cannot find the global material map object");
    }
    else
    {
        char matName[128] = { "material: " };
        char* namePtrs[2];

        U32 strLen = dStrlen(matName);
        dStrcpy(&matName[strLen], getName());
        namePtrs[0] = (char*)mapTo;
        namePtrs[1] = matName;
        pMap->addMapping(2, (const char**)namePtrs);
    }
}
