/***************************************************************************
 *   Copyright (c) 2015 Stefan Tröger <stefantroeger@gmx.net>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef Fem_FemPostFilter_H
#define Fem_FemPostFilter_H

#include <vtkCutter.h>
#include <vtkExtractGeometry.h>
#include <vtkLineSource.h>
#include <vtkPointSource.h>
#include <vtkProbeFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkWarpVector.h>

#include <App/PropertyUnits.h>

#include "FemPostObject.h"


namespace Fem
{

class FemExport FemPostFilter : public Fem::FemPostObject
{
    PROPERTY_HEADER(Fem::FemPostFilter);

public:
    /// Constructor
    FemPostFilter();
    ~FemPostFilter() override;

    App::PropertyLink Input;

    App::DocumentObjectExecReturn* execute() override;

protected:
    vtkDataObject* getInputData();

    //pipeline handling for derived filter
    struct FilterPipeline {
       vtkSmartPointer<vtkAlgorithm>                    source, target;
       vtkSmartPointer<vtkProbeFilter>                  filterSource, filterTarget;
       std::vector<vtkSmartPointer<vtkAlgorithm> >      algorithmStorage;
    };

    void addFilterPipeline(const FilterPipeline& p, std::string name);
    void setActiveFilterPipeline(std::string name);
    FilterPipeline& getFilterPipeline(std::string name);
private:
    //handling of multiple pipelines which can be the filter
    std::map<std::string, FilterPipeline> m_pipelines;
    std::string m_activePipeline;
};

class FemExport FemPostClipFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostClipFilter);

public:
    FemPostClipFilter();
    ~FemPostClipFilter() override;

    App::PropertyLink           Function;
    App::PropertyBool           InsideOut;
    App::PropertyBool           CutCells;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostClip";
    }
    short int mustExecute() const override;
    App::DocumentObjectExecReturn* execute() override;

protected:
    void onChanged(const App::Property* prop) override;

private:
    vtkSmartPointer<vtkTableBasedClipDataSet>   m_clipper;
    vtkSmartPointer<vtkExtractGeometry>         m_extractor;
};

class FemExport FemPostDataAlongLineFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostDataAlongLineFilter);

public:
    FemPostDataAlongLineFilter();
    ~FemPostDataAlongLineFilter() override;

    App::PropertyVectorDistance Point1;
    App::PropertyVectorDistance Point2;
    App::PropertyInteger        Resolution;
    App::PropertyFloatList      XAxisData;
    App::PropertyFloatList      YAxisData;
    App::PropertyString         PlotData;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostDataAlongLine";
    }
    short int mustExecute() const override;
    void GetAxisData();

protected:
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyType(Base::XMLReader& reader, const char* TypeName, App::Property* prop) override;

private:

    vtkSmartPointer<vtkLineSource>              m_line;
    vtkSmartPointer<vtkProbeFilter>             m_probe;

};

class FemExport FemPostDataAtPointFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostDataAtPointFilter);

public:
    FemPostDataAtPointFilter();
    ~FemPostDataAtPointFilter() override;

    App::PropertyVectorDistance   Center;
    App::PropertyDistance         Radius;
    App::PropertyString           FieldName;
    App::PropertyFloatList        PointData;
    App::PropertyString           Unit;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostDataAtPoint";
    }
    short int mustExecute() const override;

protected:
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;
    void GetPointData();

private:

    vtkSmartPointer<vtkPointSource>             m_point;
    vtkSmartPointer<vtkProbeFilter>             m_probe;

};

class FemExport FemPostScalarClipFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostScalarClipFilter);

public:
    FemPostScalarClipFilter();
    ~FemPostScalarClipFilter() override;

    App::PropertyBool            InsideOut;
    App::PropertyFloatConstraint Value;
    App::PropertyEnumeration     Scalars;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostScalarClip";
    }
    short int mustExecute() const override;

protected:
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;
    void setConstraintForField();

private:
    vtkSmartPointer<vtkTableBasedClipDataSet>   m_clipper;
    App::Enumeration                            m_scalarFields;
    App::PropertyFloatConstraint::Constraints   m_constraints;
};

class FemExport FemPostWarpVectorFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostWarpVectorFilter);

public:
    FemPostWarpVectorFilter();
    ~FemPostWarpVectorFilter() override;

    App::PropertyFloat        Factor;
    App::PropertyEnumeration  Vector;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostWarpVector";
    }
    short int mustExecute() const override;

protected:
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;

private:
    vtkSmartPointer<vtkWarpVector>   m_warp;
    App::Enumeration                 m_vectorFields;
};

class FemExport FemPostCutFilter : public FemPostFilter {

    PROPERTY_HEADER(Fem::FemPostCutFilter);

public:
    FemPostCutFilter();
    ~FemPostCutFilter() override;

    App::PropertyLink  Function;

    const char* getViewProviderName() const override {
        return "FemGui::ViewProviderFemPostCut";
    }
    short int mustExecute() const override;
    App::DocumentObjectExecReturn* execute() override;

protected:
    void onChanged(const App::Property* prop) override;

private:
    vtkSmartPointer<vtkCutter>   m_cutter;
};

} //namespace Fem


#endif // Fem_FemPostFilter_H
