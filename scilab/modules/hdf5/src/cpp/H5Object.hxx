/*
 * Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 * Copyright (C) 2012 - Scilab Enterprises - Calixte DENIZET
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#ifndef __H5OBJECT_HXX__
#define __H5OBJECT_HXX__

#include <hdf5.h>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <map>
#include <cstdlib>

extern "C"
{
#include "MALLOC.h"
#include "Scierror.h"
#include "api_scilab.h"
#include "localization.h"
}

#include "H5VariableScope.hxx"
#include "H5Exception.hxx"

#define __H5_INDENT_LENGTH__ 3
#define __H5_LS_LENGTH__ 25

namespace org_modules_hdf5
{
class H5AttributesList;
class H5File;

class H5Object
{
    static H5Object & root;

    bool locked;
    H5Object & parent;
    int scilabId;

    friend class H5AttributesList;
    friend class H5LinkList;
    friend class H5Dataset;

public :

    H5Object(H5Object & _parent);
    H5Object(H5Object & _parent, const std::string & _name);
    virtual ~H5Object();

    virtual hid_t getH5Id() const;
    virtual H5AttributesList & getAttributes();

    virtual H5O_info_t getInfo() const
    {
        H5O_info_t info;
        H5Oget_info(getH5Id(), &info);

        return info;
    }

    virtual bool isFile() const
    {
        return false;
    }

    virtual bool isGroup() const
    {
        return false;
    }

    virtual bool isAttribute() const
    {
        return false;
    }

    virtual bool isReference() const
    {
        return false;
    }

    virtual bool isDataspace() const
    {
        return false;
    }

    virtual bool isDataset() const
    {
        return false;
    }

    virtual bool isType() const
    {
        return false;
    }

    virtual bool isList() const
    {
        return false;
    }

    virtual haddr_t getAddr() const
    {
        return getInfo().addr;
    }

    virtual const std::string & getName() const
    {
        return name;
    }

    virtual const std::string getBaseName() const
    {
        std::string::size_type pos = name.find_last_of('/');
        if (pos == std::string::npos)
        {
            return name;
        }
        else
        {
            return name.substr(pos + 1);
        }
    }

    virtual std::string getCompletePath() const;
    virtual std::string dump(std::map<haddr_t, std::string> & alreadyVisited, const unsigned int indentLevel = 0) const
    {
        return "";
    }

    virtual std::string ls() const
    {
        return "";
    }

    virtual void ls(std::vector<std::string> & name, std::vector<std::string> & type) const
    {

    }

    virtual void printLsInfo(std::ostringstream & os) const
    {
        return;
    }

    virtual std::string toString() const
    {
        return toString(0);
    }
    virtual std::string toString(const unsigned int indentLevel) const
    {
        return "";
    }
    virtual void getAccessibleAttribute(const std::string & _name, const int pos, void * pvApiCtx) const;
    virtual void getAccessibleAttribute(const double index, const int pos, void * pvApiCtx) const
    {
        throw H5Exception(__LINE__, __FILE__, _("Invalid operation"));
    }

    virtual void setAccessibleAttribute(const std::string & name, const int pos, void * pvApiCtx) const
    {
        throw H5Exception(__LINE__, __FILE__, _("Invalid operation"));
    }

    virtual void setAccessibleAttribute(const double index, const int pos, void * pvApiCtx) const
    {
        throw H5Exception(__LINE__, __FILE__, _("Invalid operation"));
    }

    void setScilabId(const int id)
    {
        scilabId = id;
    }

    H5Object & getParent() const
    {
        return parent;
    }
    H5File & getFile() const;

    virtual void createOnScilabStack(int pos, void * pvApiCtx) const;
    virtual void createInScilabList(int * list, int stackPos, int pos, void * pvApiCtx) const;
    bool isRoot() const
    {
        return this == &root;
    }

    static std::string getIndentString(const unsigned int indentLevel)
    {
        return std::string((size_t)(__H5_INDENT_LENGTH__ * indentLevel), ' ');
    }

    static H5Object & getRoot()
    {
        return root;
    }

    static void cleanAll()
    {
        root.locked = true;
        for (std::set<H5Object *>::iterator it = root.children.begin(); it != root.children.end(); it++)
        {
            delete *it;
        }
        root.locked = false;
        H5VariableScope::clearScope();
    }

    static void getResizedString(std::string & str)
    {
        if (str.length() < __H5_LS_LENGTH__)
        {
            str.resize(__H5_LS_LENGTH__, ' ');
        }
    }

    static H5Object & getObject(H5Object & parent, hid_t obj);
    static H5Object & getObject(H5Object & parent, const std::string & name);
    static void getLinksInfo(const H5Object & obj, std::vector<std::string> & linksName, std::vector<std::string> & types, std::vector<std::string> & linksType);

protected :

    typedef struct
    {
        H5Object * parent;
        std::vector<std::string> * name;
        std::vector<std::string> * type;
    } OpDataGetLs;

    const std::string name;
    std::set<H5Object *> children;
    void registerChild(H5Object * child)
    {
        if (!locked) children.insert(child);
    }
    void unregisterChild(H5Object * child)
    {
        if (!locked) children.erase(child);
    }

    static herr_t getLsAttributes(hid_t location_id, const char * attr_name, const H5A_info_t * ainfo, void * op_data);

private :

    H5Object() : parent(*this) { }

    typedef struct
    {
        std::vector<std::string> * name;
        std::vector<std::string> * type;
        std::vector<std::string> * linktype;
    } LinksInfo_;

    static herr_t iterateGetInfo(hid_t g_id, const char * name, const H5L_info_t * info, void * op_data);
};
}

#undef __H5_INDENT_LENGTH__
#undef __H5_LS_LENGTH__

#endif // __H5OBJECT_HXX__