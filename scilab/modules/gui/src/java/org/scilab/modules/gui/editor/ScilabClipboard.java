/*
 * Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 * Copyright (C) 2012 - Pedro Arthur dos S. Souza
 * Copyright (C) 2012 - Caio Lucas dos S. Souza
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

package org.scilab.modules.gui.editor;

import org.scilab.modules.graphic_objects.graphicObject.GraphicObjectProperties;
import org.scilab.modules.gui.editor.PolylineHandler;

/**
* Implements clipboard between figures for polylines.
*
* @author Caio Souza <caioc2bolado@gmail.com>
* @author Pedro Souza <bygrandao@gmail.com>
*
* @since 2012-06-01
*/

public class ScilabClipboard {

    static ScilabClipboard instance = null;
    String objectUid = null;
    Integer copiedColor = 0;
    boolean needDuplication = false;
    String copyStyle;

    public static ScilabClipboard getInstance() {
        if (instance == null) {
            instance = new ScilabClipboard();
        }
        return instance;
    }

    /**
    * Copy a object to the clipboad.
    * @param uid object unique identifier.
    */
    public void copy(String uid) {
        objectUid = uid;
        needDuplication = true;
    }

    /**
    * Paste the copied object to the given axes.
    * @param figure Figure unique identifier.
    * @param position Vector with mouse position x and y.
    * @retrun The UID from pasted object
    */
    public String paste(String figure, Integer[] position) {
        String object = objectUid;
        /*We store only the uid, so we need check if the object exists*/
        if (!canPaste()) {
            return null;
        }

        String axesFrom = (new ObjectSearcher()).searchParent(object, GraphicObjectProperties.__GO_AXES__);
        if (axesFrom == null) {
            return null;
        }

        if (needDuplication == true ) {
            object = CommonHandler.duplicate(objectUid);
        } else {
            CommonHandler.cut(object);
        }

        CommonHandler.setColor(object, copiedColor);

        String axesTo = AxesHandler.clickedAxes(figure, position);
        if (axesTo != null) { /* If there is an axes in the clicked position then adjust the bounds, make the axes visible and paste */
            AxesHandler.axesBound(axesFrom, axesTo);
            AxesHandler.setAxesVisible(axesTo);
            CommonHandler.insert(axesTo, object);
            objectUid = null;
            return object;
        } else { /* If doesn't exists an axes will duplicate the origin axes */
            axesTo = AxesHandler.duplicateAxes(axesFrom);
            if (axesTo != null) { /* If duplicated sucessfull then adjust the bounds and paste */
                AxesHandler.axesBound(axesFrom, axesTo);
                CommonHandler.insert(axesTo, object);
                objectUid = null;
                return object;
            }
            return null;
        }
    }

    /**
    * Cut an object to the clipboad.
    * @param object object unique identifier.
    */
    public void cut(String object) {
        objectUid = object;
        needDuplication = false;
    }

    /**
    * Check if there is any polyline copied/cuted to be pasted.
    * @return True if can be pasted, false otherwise.
    */
    public boolean canPaste() {
        if (!CommonHandler.objectExists(objectUid)) {
            objectUid = null;
            return false;
        }
        return true;
    }

    /**
    * Used to store the color of the copied polyline.
    * @param color Polyline color.
    */
    public void setCopiedColor(Integer color) {
        copiedColor = color;
    }

    /**
    * Get The current object in the clipboard
    *
    * @return Current object in the clipboard
    */
    public String getCurrentObject() {

        return objectUid;
    }

    /**
    * Check if the object where the style will be copied exists
    */
    public boolean canPasteStyle() {

        if (!CommonHandler.objectExists(copyStyle)) {
            copyStyle = null;
            return false;
        }
        return true;
    }

    /**
    * Copy store the object to copy style
    *
    * @param object The axes to store
    */
    public void copyStyle(String objectUID) {

        copyStyle = objectUID;
    }

    /**
    * Paste the Style of the object in the clipboard to a new object
    *
    * @param object The object to recieve the style
    * @return The new axes pasted
    */
    public String pasteStyle(String objectUID) {

        if (!canPasteStyle()) {
            return null;
        }
        String newAxes = AxesHandler.cloneAxesWithStyle(copyStyle);
        String figureFrom = CommonHandler.getParentFigure(copyStyle);
        String figureTo = CommonHandler.getParentFigure(objectUID);
        CommonHandler.cloneColorMap(figureFrom, figureTo);
        CommonHandler.cloneBackgroundColor(figureFrom, figureTo);
        AxesHandler.pasteAxesStyle(newAxes, objectUID);
        return newAxes;
    }

}
