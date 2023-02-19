#include "gui/game/guiAchievementPopupCtrl.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(GuiAchievementPopupCtrl);

//------------------------------------------------------------------------------

GuiAchievementPopupCtrl::GuiAchievementPopupCtrl()
{
    mBitmapName = StringTable->insert("");
    mTextureObject = NULL;
    mTitle = StringTable->insert("");
}

//------------------------------------------------------------------------------

void GuiAchievementPopupCtrl::initPersistFields()
{
    Parent::initPersistFields();

    addField("bitmap", TypeFilename, Offset(mBitmapName, GuiAchievementPopupCtrl));
}

//------------------------------------------------------------------------------

bool GuiAchievementPopupCtrl::onWake()
{
    if (!Parent::onWake())
        return false;

    setBitmap(mBitmapName);

    mProfile->constructBitmapArray();

    return true;
}

void GuiAchievementPopupCtrl::setBitmap(const char *name)
{
    mBitmapName = StringTable->insert(name);
    if (*mBitmapName)
        mTextureObject.set(mBitmapName, &GFXDefaultGUIProfile);
    else
        mTextureObject = NULL;

    setUpdate();
}

void GuiAchievementPopupCtrl::setTitle(const char *title)
{
    mTitle = StringTable->insert(title);
    setUpdate();
}

//------------------------------------------------------------------------------

void GuiAchievementPopupCtrl::onRender(Point2I offset, const RectI &updateRect)
{
    if (mTextureObject)
    {
        GFX->clearBitmapModulation();

        RectI rect(offset.x + 6, offset.y + 4, 64, 64);
        GFX->drawBitmapStretch(mTextureObject, rect);
    }

    if (mProfile->mBorder || !mTextureObject)
    {
        RectI rect(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);
        GFX->drawRect(rect, mProfile->mBorderColor);
    }

    renderJustifiedText(offset, mBounds.extent, mTitle);

    renderChildControls(offset, updateRect);
}

ConsoleMethod(GuiAchievementPopupCtrl, setBitmap, void, 3, 3, "(string filename)"
    "Set the bitmap displayed in the control. Note that it is limited in size, to 256x256.")
{
    char fileName[1024];
    Con::expandScriptFilename(fileName, sizeof(fileName), argv[2]);
    object->setBitmap(fileName);
}

ConsoleMethod(GuiAchievementPopupCtrl, setTitle, void, 3, 3, "(string title)"
    "Set the title of the achievement.")
{
    object->setTitle(argv[2]);
}
