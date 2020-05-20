/**
 * @file ObjectAttribute.h
 * @brief Utilities for handling the object attribute memory in the GBA
 * 
 * The object attribute memory is a slightly convoluted. Provides a bunch of helpful methods
 * to make handling it much safer and easier.
 * 
 * Also, automatically provides a buffer for use and some copy methods.
 * 
 * @defgroup OBJECT_ATTRIBUTE Object attribute management
 * @{
 */

#pragma once

#include "GbaTypes.h"

/**
 * @brief The attribute data for a singular object.
 * 
 * You shouldn't touch the internal attributes, but should instead use the ObjectAttribute_* methods provided in this
 * module to manipulate this.
 * 
 * The ObjectAttribute memory is interlaced with the ObjectAffine memory, and these are copied together when calling
 * ObjectAttributeBuffer_CopyBufferToMemory().
 */
struct ObjectAttribute
{
    /**
     * @brief Attribute 0
     * 
     * The 16 bits of attribute 0 refer to the following:
     * 
     * <table>
     * <tr><th>Bits</th><th>Description</th></tr>
     * <tr>
     *  <td>0-7</td>
     *  <td><b>The Y coordinate</b> of the sprite. Modified with ObjectAttribute_SetPos() </td>
     * </tr>
     * <tr>
     *  <td>A-B</td>
     *  <td>
     *   <b>(Affine) display mode</b>. Use to hide the sprite or set the affine mode.
     *   Modified with ObjectAttribute_SetDisplayMode(). Possible values described
     *   in ObjectAttributeDisplayMode 
     *  </td>
     * </tr>
     * <tr>
     *  <td>C</td>
     *  <td><b>Mosaic effect</b>. Whether the mosaic effect is enabled on this sprite. Modified with ObjectAttribute_SetMosaicEnabled()
     * </tr>
     * <tr>
     *  <td>D</td>
     *  <td>
     *   <b>Colour mode</b>. Which colour mode to use for this sprite. 16 colours (4bpp) or 256 colours (8bpp).
     *   Set with ObjectAttribute_SetColourMode(), see ObjectAttributeColourMode enum for possible values.
     * </tr>
     * <tr>
     *  <td>E-F</td>
     *  <td>
     *   <b>Sprite shape</b>. The sprite size + the sprite shape determines the sprites real size.
     *   Set with ObjectAttribute_SetShape() and see ObjectAttributeShape for possible values.
     *  </td>
     * </tr>
     * </table>
     */
    u16 attr0;
    /**
     * @brief Attribute 1
     * 
     * The 16 bits of attribute 1 refer to the following:
     * 
     * <table>
     * <tr><th>Bits</th><th>Description</th></tr>
     * <tr>
     *  <td>0-8</td>
     *  <td><b>The X coordinate</b> of the sprite. Modified with ObjectAttribute_SetPos() </td>
     * </tr>
     * <tr>
     *  <td>9-D</td>
     *  <td>
     *   <b>Affine index</b>. Specifies which affine index this spriteuses. Valid <i>only</i> if the DisplayMode is
     *   ObjectAttributeDisplayMode_Affine or ObjectAttributeDisplayMode_DoubleRender
     * 
     *   Changed with ObjectAttribute_SetAffineIndex()
     *  </td>
     * </tr>
     * <tr>
     *  <td>C</td>
     *  <td>
     *   <b>Horizontal flip</b>. Whether this sprite should be flipped horizontally. Valid <i>only</i> if
     *   the display mode is ObjectAttributeDisplayMode_Normal or ObjectAttributeDisplayMode_Hidden.
     * 
     *   Changed with ObjectAttribute_SetHFlip()
     *  </td>
     * </tr>
     * <tr>
     *  <td>D</td>
     *  <td>
     *   <b>Vertical flip</b>. Whether this sprite should be flipped horizontally. Valid <i>only</i> if
     *   the display mode is ObjectAttributeDisplayMode_Normal or ObjectAttributeDisplayMode_Hidden.
     * 
     *   Changed with ObjectAttribute_SetVFlip()
     *  </td>
     * </tr>
     * <tr>
     *  <td>E-F</td>
     *  <td>
     *   <b>Sprite size</b>. Combined with the sprite shape described above, determines the actual sprite
     *   size. See the table in the ObjectAttribute documentation for the sprites real size.
     * 
     *   Changed with ObjectAttribute_SetSize().
     * </td>
     * </tr>
     * </table>
     */
    u16 attr1;
    /**
     * @brief Attribute 2
     * 
     * The 16 bits of attribute 2 refer to the following:
     * 
     * <table>
     * <tr><th>Bits</th><th>Description</th></tr>
     * <tr>
     *  <td>0-9</td>
     *  <td>
     *   <b>The tile index</b> of the sprite. Modified with ObjectAttribute_SetTile().
     *   Note that in bitmap modes, this must be 512 or higher, and in non-bitmap modes depends on
     *   the colour index of the tiles, but can be no higher then 1024.
     *  </td>
     * </tr>
     * <tr>
     *  <td>A-B</td>
     *  <td>
     *   <b>Priority</b>. Higher priorities are drawn first (and can be drawn over by later sprites and backgrounds).
     *   Sprites cover backgrounds with the same priority and for sprites with the same priority, the ones which
     *   appear later in the object attribute list are draw first. Change with ObjectAttribute_SetPriority()
     *  </td>
     * </tr>
     * </table>
     */
    u16 attr2;
    /**
     * @brief Filler to make this struct the correct size.
     * 
     * Will be filled with ObjectAffine information so <b>DO NOT TOUCH</b>.
     * 
     * @internal
     */
    s16 fill;
} LOSTGBA_ALIGN(4);

/**
 * @brief Sets the position of the sprite
 * @param attr The sprite to change the position of
 * @param x The x coordinate of the top left. Can be out of screen / negative
 * @param y The y coordinate of the top left. Can be out of screen / negative.
 */
void ObjectAttribute_SetPos(struct ObjectAttribute *attr, int x, int y);

/** The display mode which controls how the sprite is rendered */
enum ObjectAttributeDisplayMode
{
    /** Normal rendering */
    ObjectAttributeDisplayMode_Normal,
    /** This sprite is an affine sprite, with affine matrix specified by ObjectAttribute_SetAffineIndex() */
    ObjectAttributeDisplayMode_Affine,
    /** This sprite should not be displayed */
    ObjectAttributeDisplayMode_Hidden,
    /** Affine sprite using double render area */
    ObjectAttributeDisplayMode_DoubleRender,
};
/** Sets the display mode of the sprite @p attr */
void ObjectAttribute_SetDisplayMode(struct ObjectAttribute *attr, enum ObjectAttributeDisplayMode displayMode);

/** Flags for special effects */
enum ObjectAttributeGraphicsMode
{
    /** Normal rendering */
    ObjectAttributeGraphicsMode_Normal,
    /** Enables alpha blending */
    ObjectAttributeGraphicsMode_Alpha,
};
/** Sets the sprite @p attr graphics effect */
void ObjectAttribute_SetGraphicsMode(struct ObjectAttribute *attr, enum ObjectAttributeGraphicsMode graphicsMode);

/** Set whether @p attr should be displayed with a mosaic effect */
void ObjectAttribute_SetMosaicEnabled(struct ObjectAttribute *attr, bool enabled);

/** The colour mode to render the sprite with */
enum ObjectAttributeColourMode
{
    /** 4PP uses a 16 colour palette. Should specify which palette bank to use with this sprite with ObjectAttribute_SetPaletteBank() */
    ObjectAttributeColourMode_4PP,
    /** 8PP uses a 256 colour palette. The palette bank is ignored in this case */
    ObjectAttributeColourMode_8PP,
};
/** Sets the colour mode for @p attr */
void ObjectAttribute_SetColourMode(struct ObjectAttribute *attr, enum ObjectAttributeColourMode colourMode);

/** The shape of the sprite. This combined with ObjectAttributeSize determines the actual size of the sprite */
enum ObjectAttributeShape
{
    /** The sprite is size x size in size */
    ObjectAttributeShape_Square,
    /** The sprite is 16 x 8, 32 x 8, 32 x 16 or 64 x 32 px in size depending on the value of size */
    ObjectAttributeShape_Wide,
    /** The sprite is 8 x 16, 8 x 32, 16 x 32 or 32 x 64 px in size depending on the value of size */
    ObjectAttributeShape_Tall,
};
/** Sets the shape for @p attr */
void ObjectAttribute_SetShape(struct ObjectAttribute *attr, enum ObjectAttributeShape shape);

/** Sets the index of the ObjectAffine used for this sprite. Only valid if ObjectAttributeDisplayMode is ObjectAttributeDisplayMode_Affine or ObjectAttributeDisplayMode_DoubleRender */
void ObjectAttribute_SetAffineIndex(struct ObjectAttribute *attr, u16 affineIndex);
/** Sets whether the sprite should be horizontally flipped. Only valid if ObjectAttributeDisplayMode is ObjectAttributeDisplayMode_Normal or ObjectAttributeDisplayMode_Hidden */
void ObjectAttribute_SetHFlip(struct ObjectAttribute *attr, bool hflip);
/** Sets whether the sprite should be vertically flipped. Only valid if ObjectAttributeDisplayMode is ObjectAttributeDisplayMode_Normal or ObjectAttributeDisplayMode_Hidden */
void ObjectAttribute_SetVFlip(struct ObjectAttribute *attr, bool vflip);

/** The 'size' of the sprite. See the definition of ObjectAttributeShape to understand the way this interacts with that field */
enum ObjectAttributeSize
{
    /** Sprite is 8px in size */
    ObjectAttributeSize_8,
    /** Sprite is 16px in size */
    ObjectAttributeSize_16,
    /** Sprite is 32px in size */
    ObjectAttributeSize_32,
    /** Sprite is 64px in size */
    ObjectAttributeSize_64,
};
/** Sets the 'size' of the sprite @p attr. Find the interaction between this and ObjectAttributeShape in the ObjectAttributeShape documentation */
void ObjectAttribute_SetSize(struct ObjectAttribute *attr, enum ObjectAttributeSize size);
/** Sets the desired tile of this sprite */
void ObjectAttribute_SetTile(struct ObjectAttribute *attr, u32 tileId);
/**
 * @brief Sets the render priority for this sprite.
 * 
 * Higher priorities are drawn first (and can be drawn over by later sprites and backgrounds).
 *   Sprites cover backgrounds with the same priority and for sprites with the same priority, the ones which
 *   appear later in the object attribute list are draw first. Change with ObjectAttribute_SetPriority()
 */
void ObjectAttribute_SetPriority(struct ObjectAttribute *attr, u16 priority);
/**
 * @brief Choose which palette bank this sprite should use
 * 
 * This is only used if the sprite is rendered in 16 colour mode, otherwise the sprite colour palette is used.
 */
void ObjectAttribute_SetPaletteBank(struct ObjectAttribute *attr, u16 paletteBank);

struct ObjectAffine
{
    u16 fill0[3];
    s16 pa;
    u16 fill1[3];
    s16 pb;
    u16 fill2[3];
    s16 pc;
    u16 fill3[3];
    s16 pd;
} LOSTGBA_ALIGN(4);

/** The total number of object attributes available. */
#define ObjectAttributeBuffer_Length 128
#define ObjectAffineBuffer_Length 32

/**
 * @brief An in memory buffer of the object attribute memory
 * 
 * This buffer is needed because while the screen is rendering, you cannot change the value of anything
 * in the object attribute memory, so it is best to prepare this during rendering time and when the
 * screen is ready to be updated, call ObjectAttributeBuffer_CopyBufferToMemory()
 */
extern struct ObjectAttribute objectAttributeBuffer[ObjectAttributeBuffer_Length];
extern struct ObjectAffine *objectAffineBuffer;

/** 
 * Copies the contents of objectAttributeBuffer (and objectAffineBuffer) to the object attribute memory.
 * Probably want to call this every frame
 */
void ObjectAttributeBuffer_CopyBufferToMemory(void);

/** @} */