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

import org.scilab.modules.graphic_objects.graphicController.GraphicController;
import org.scilab.modules.graphic_objects.graphicObject.GraphicObjectProperties;


import java.util.*;

/**
* Serach for objects in the given object child tree.
*
* @author Caio Souza <caioc2bolado@gmail.com>
* @author Pedro Souza <bygrandao@gmail.com>
*
* @since 2012-06-01
*/

public class ObjectSearcher {

    private List<String> objects = new ArrayList<String>();
    private String type;
    private String[] types;

    /**
    * Search for the given object type.
    * @param rootUid    Root object to search.
    * @param objType    Object type.
    * @return             A vector with the uid of the objects found.
    */
    public String[] search( String rootUid, String objType) {

        type = objType;
        objects.clear();
        getObjects(rootUid);
        if (objects.size() != 0) {
            String[] ret = new String[objects.size()];
            for (int i = 0; i < objects.size(); ++i) {
                ret[i] = objects.get(i);
            }
            return ret;
        }

        return null;
    }

    /**
    * Search for the given object type in object parent path.
    *
    * @param uid Object unique identifier.
    * @param type Object type.
    * @return The first parent found with the given type, or null if none is found.
    */
    public String searchParent(String uid, String type) {

        if (uid != null) {
            String parent = (String)GraphicController.getController().getProperty( uid, GraphicObjectProperties.__GO_PARENT__);
            if (type == (String)GraphicController.getController().getProperty(parent, GraphicObjectProperties.__GO_TYPE__)) {
                return parent;
            } else {
                return searchParent(parent, type);
            }
        }
        return null;
    }

    private void getObjects(String uid) {

        Integer childCount = (Integer)GraphicController.getController().getProperty(uid, GraphicObjectProperties.__GO_CHILDREN_COUNT__);
        String[] childUid = (String[])GraphicController.getController().getProperty(uid, GraphicObjectProperties.__GO_CHILDREN__);

        for (Integer i = 0; i < childCount; ++i ) {
            String objType = (String)GraphicController.getController().getProperty(childUid[i], GraphicObjectProperties.__GO_TYPE__);
            if ( objType == type ) {
                objects.add( childUid[i] );
            } else {
                getObjects(childUid[i]);
            }
        }
    }

    private void getMultipleObjects(String uid) {

        Integer childCount = (Integer)GraphicController.getController().getProperty(uid, GraphicObjectProperties.__GO_CHILDREN_COUNT__);
        String[] childUid = (String[])GraphicController.getController().getProperty(uid, GraphicObjectProperties.__GO_CHILDREN__);

        for (Integer i = 0; i < childCount; ++i ) {
            String objType = (String)GraphicController.getController().getProperty(childUid[i], GraphicObjectProperties.__GO_TYPE__);

            boolean found = false;

            for (Integer j = 0; j < types.length; ++j) {
                if (objType == types[j]) {
                    objects.add(childUid[i]);
                    found = true;
                    break;
                }
            }
            if (!found) {
                getMultipleObjects(childUid[i]);
            }
        }
    }

    public String[] searchMultiple(String root, String[] objTypes) {

        types = objTypes;
        objects.clear();
        getMultipleObjects(root);
        if (objects.size() != 0){
            String[] ret = new String[objects.size()];
            for (int i = 0; i < objects.size(); ++i) {
                 ret[i] = objects.get(i);
            }
            return ret;
        }
        return null;
    }
}
