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

#include "H5StringData.hxx"

namespace org_modules_hdf5
{

H5StringData::H5StringData(H5Object & _parent, const hsize_t _totalSize, const hsize_t _stringSize, const hsize_t _ndims, const hsize_t * _dims, char * _data, const hsize_t _stride, const size_t _offset, const bool _dataOwner) : H5Data(_parent, _totalSize, _stringSize, _ndims, _dims, _data, _stride == 0 ? _stringSize : _stride, _offset, _dataOwner)
{
    char * __data = static_cast<char *>(_data);
    transformedData = new char*[totalSize];

    if (stride == 0)
    {
        *transformedData = __data;
        for (unsigned int i = 1; i < (unsigned int)totalSize; i++)
        {
            transformedData[i] = transformedData[i - 1] + dataSize;
        }
    }
    else
    {
        for (unsigned int i = 0; i < (unsigned int)totalSize; i++)
        {
            transformedData[i] = __data + offset;
            __data += stride;
        }
    }
}

H5StringData::H5StringData(H5Object & _parent, const hsize_t _totalSize, const hsize_t _stringSize, const hsize_t _ndims, const hsize_t * _dims, char ** _data, const hsize_t _stride, const size_t _offset, const bool _dataOwner) : H5Data(_parent, _totalSize, _stringSize, _ndims, _dims, _data, _stride == 0 ? _stringSize : _stride, _offset, _dataOwner), transformedData(0)
{

}

H5StringData::~H5StringData()
{

    if (transformedData)
    {
        delete[] transformedData;
    }
    else
    {
        char ** _data = reinterpret_cast<char **>(getData());
        for (hsize_t i = 0; i < totalSize; i++)
        {
            if (_data[i])
            {
                free(_data[i]);
            }
        }
    }
}

void * H5StringData::getData() const
{
    if (transformedData)
    {
        return transformedData;
    }
    else
    {
        return H5Data::getData();
    }
}

void H5StringData::printData(std::ostream & os, const unsigned int pos, const unsigned int indentLevel) const
{
    char * str = static_cast<char **>(getData())[pos];
    if (str)
    {
        os << "\"" << str << "\"";
    }
    else
    {
        os << "NULL";
    }
}

void H5StringData::toScilab(void * pvApiCtx, const int lhsPosition, int * parentList, const int listPosition) const
{
    SciErr err;
    char ** _data = static_cast<char **>(getData());

    if (ndims == 0)
    {
        H5BasicData<char *>::create(pvApiCtx, lhsPosition, 1, 1, _data, parentList, listPosition);
    }
    else if (ndims == 1)
    {
        H5BasicData<char *>::create(pvApiCtx, lhsPosition, 1, *dims, _data, parentList, listPosition);
    }
    else
    {
        char ** newData = new char *[totalSize];
        if (ndims == 2)
        {
            H5DataConverter::C2FHypermatrix(2, dims, 0, _data, newData);
            H5BasicData<char *>::create(pvApiCtx, lhsPosition, dims[0], dims[1], newData, parentList, listPosition);
        }
        else
        {
            int * list = getHypermatrix(pvApiCtx, lhsPosition, parentList, listPosition);
            H5DataConverter::C2FHypermatrix(ndims, dims, (int)totalSize, _data, newData);
            H5BasicData<char *>::create(pvApiCtx, lhsPosition, 1, (int)totalSize, newData, parentList, listPosition);
        }
        delete[] newData;
    }
}

std::string H5StringData::dump(std::map<haddr_t, std::string> & alreadyVisited, const unsigned int indentLevel) const
{
    return H5DataConverter::dump(alreadyVisited, indentLevel, ndims, dims, *this);
}
}