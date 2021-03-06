// ***************************************************************************
//
//   Generated automatically by genwrapper.
//   Please DO NOT EDIT this file!
//
// ***************************************************************************

#include <osgIntrospection/ReflectionMacros>
#include <osgIntrospection/TypedMethodInfo>
#include <osgIntrospection/StaticMethodInfo>
#include <osgIntrospection/Attributes>

#include <osg/CopyOp>
#include <osg/Matrix>
#include <osg/Object>
#include <osgART/Marker>

// Must undefine IN and OUT macros defined in Windows headers
#ifdef IN
#undef IN
#endif
#ifdef OUT
#undef OUT
#endif

BEGIN_ENUM_REFLECTOR(osgART::Marker::MarkerType)
	I_DeclaringFile("osgART/Marker");
	I_EnumLabel(osgART::Marker::ART_UNKOWN);
	I_EnumLabel(osgART::Marker::ART_SINGLE);
	I_EnumLabel(osgART::Marker::ART_MULTI);
	I_EnumLabel(osgART::Marker::ART_NFT);
	I_EnumLabel(osgART::Marker::ART_BAZAR);
END_REFLECTOR

BEGIN_OBJECT_REFLECTOR(osgART::Marker)
	I_DeclaringFile("osgART/Marker");
	I_BaseType(osg::Referenced);
	I_Method0(osg::Object *, cloneType,
	          Properties::VIRTUAL,
	          __osg_Object_P1__cloneType,
	          "",
	          "");
	I_Method1(osg::Object *, clone, IN, const osg::CopyOp &, copyop,
	          Properties::VIRTUAL,
	          __osg_Object_P1__clone__C5_osg_CopyOp_R1,
	          "",
	          "");
	I_Method1(bool, isSameKindAs, IN, const osg::Object *, obj,
	          Properties::VIRTUAL,
	          __bool__isSameKindAs__C5_osg_Object_P1,
	          "",
	          "");
	I_Method0(const char *, libraryName,
	          Properties::VIRTUAL,
	          __C5_char_P1__libraryName,
	          "",
	          "");
	I_Method0(const char *, className,
	          Properties::VIRTUAL,
	          __C5_char_P1__className,
	          "",
	          "");
	I_Constructor0(____Marker,
	               "default constructor. ",
	               "");
	I_ConstructorWithDefaults2(IN, const osgART::Marker &, container, , IN, const osg::CopyOp &, copyop, osg::CopyOp::SHALLOW_COPY,
	                           ____Marker__C5_Marker_R1__C5_osg_CopyOp_R1,
	                           "Copy constructor. ",
	                           "Needed for cloning in OSG ");
	I_Method0(osgART::Marker::MarkerType, getType,
	          Properties::VIRTUAL,
	          __MarkerType__getType,
	          "Return the type of the Marker. ",
	          "type of marker  ");
	I_Method0(bool, valid,
	          Properties::VIRTUAL,
	          __bool__valid,
	          "Determine if the marker is visible. ",
	          "Return the validity of the marker, mainly valid with no occlusion, no loss of tracking, in the FOV of the camera, correct recognition, etc. the validity of the marker (true if valid).  ");
	I_Method0(const osg::Matrix &, getTransform,
	          Properties::NON_VIRTUAL,
	          __C5_osg_Matrix_R1__getTransform,
	          "Obtain the transformation of the marker. ",
	          "Return return a 4x4 transformation of the marker (european order). a 4x4 matrix  ");
	I_Method1(void, setName, IN, const std::string &, name,
	          Properties::NON_VIRTUAL,
	          __void__setName__C5_std_string_R1,
	          "set name of the marker ",
	          " param name identifier name for the marker  ");
	I_Method0(const std::string &, getName,
	          Properties::NON_VIRTUAL,
	          __C5_std_string_R1__getName,
	          "retrieve name of the marker ",
	          "marker name  ");
	I_Method1(void, setActive, IN, bool, active,
	          Properties::VIRTUAL,
	          __void__setActive__bool,
	          "set the respective marker active ",
	          " param active set to TRUE if the marker should be tracked  ");
	I_Method0(bool, active,
	          Properties::NON_VIRTUAL,
	          __bool__active,
	          "retrieve activity status from the marker ",
	          "TRUE if the marker is active  ");
	I_Method0(float, getConfidence,
	          Properties::NON_VIRTUAL,
	          __float__getConfidence,
	          "",
	          "");
	I_ProtectedMethod1(void, updateTransform, IN, const osg::Matrix &, transform,
	                   Properties::NON_VIRTUAL,
	                   Properties::NON_CONST,
	                   __void__updateTransform__C5_osg_Matrix_R1,
	                   "Internal update of matrix. ",
	                   "");
	I_SimpleProperty(bool, Active, 
	                 0, 
	                 __void__setActive__bool);
	I_SimpleProperty(float, Confidence, 
	                 __float__getConfidence, 
	                 0);
	I_SimpleProperty(const std::string &, Name, 
	                 __C5_std_string_R1__getName, 
	                 __void__setName__C5_std_string_R1);
	I_SimpleProperty(const osg::Matrix &, Transform, 
	                 __C5_osg_Matrix_R1__getTransform, 
	                 0);
	I_SimpleProperty(osgART::Marker::MarkerType, Type, 
	                 __MarkerType__getType, 
	                 0);
END_REFLECTOR

