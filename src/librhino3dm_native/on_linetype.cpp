#include "stdafx.h"

RH_C_FUNCTION ON_Linetype* ON_Linetype_New(const ON_Linetype* pConstLinetype)
{
  if (pConstLinetype)
    return new ON_Linetype(*pConstLinetype);
  return new ON_Linetype();
}

RH_C_FUNCTION void ON_Linetype_Default(ON_Linetype* pLinetype)
{
  if( pLinetype )
    *pLinetype = ON_Linetype::Continuous;
}

RH_C_FUNCTION void ON_Linetype_GetLinetypeName(const ON_Linetype* pLinetype, CRhCmnStringHolder* pStringHolder)
{
  if( pLinetype && pStringHolder)
    pStringHolder->Set( pLinetype->Name() );
}

RH_C_FUNCTION void ON_Linetype_SetLinetypeName(ON_Linetype* pLinetype, const RHMONO_STRING* _name)
{
  INPUTSTRINGCOERCE(name, _name);
  if( pLinetype )
  {
    pLinetype->SetName(name);
  }
}

enum LinetypeInteger : int
{
  ltiSegmentCount = 0,
  ltiLinetypeIndex = 1,
  ltiLineCap = 2,
  ltiLineJoin = 3
};


RH_C_FUNCTION int ON_Linetype_GetInt(const ON_Linetype* pLinetype, enum LinetypeInteger which)
{
  int rc = -1;
  if( pLinetype )
  {
    switch(which)
    {
    case ltiSegmentCount:
      rc = pLinetype->SegmentCount();
      break;
    case ltiLineCap:
      rc = (int)pLinetype->LineCapStyle();
      break;
    case ltiLineJoin:
      rc = (int)pLinetype->LineJoinStyle();
      break;
    default:
      break;
    }
  }
  return rc;
}

RH_C_FUNCTION void ON_Linetype_SetInt(ON_Linetype* pLinetype, enum LinetypeInteger which, int val)
{
  if( pLinetype )
  {
    switch(which)
    {
    case ltiLinetypeIndex:
      pLinetype->SetIndex(val);
      break;
    case ltiLineCap:
      pLinetype->SetLineCapStyle(ON::LineCapStyleFromUnsigned(val));
      break;
    case ltiLineJoin:
      pLinetype->SetLineJoinStyle(ON::LineJoinStyleFromUnsigned(val));
      break;
    default:
      break;
    }
  }
}

RH_C_FUNCTION double ON_Linetype_PatternLength(const ON_Linetype* pLinetype)
{
  double rc = 0;
  if( pLinetype )
    rc = pLinetype->PatternLength();
  return rc;
}

RH_C_FUNCTION int ON_Linetype_AppendSegment(ON_Linetype* pLinetype, double length, bool isSolid)
{
  int rc = -1;
  if( pLinetype )
  {
    ON_LinetypeSegment seg;
    seg.m_length = length;
    seg.m_seg_type = isSolid?ON_LinetypeSegment::eSegType::stLine : ON_LinetypeSegment::eSegType::stSpace;
    rc = pLinetype->AppendSegment(seg);
  }
  return rc;
}

RH_C_FUNCTION bool ON_Linetype_RemoveSegment(ON_Linetype* pLinetype, int index)
{
  bool rc = false;
  if( pLinetype )
  {
    rc = pLinetype->RemoveSegment(index);
  }
  return rc;
}

RH_C_FUNCTION bool ON_Linetype_SetSegment(ON_Linetype* pLinetype, int index, double length, bool isSolid)
{
  bool rc = false;
  if( pLinetype )
  {
    ON_LinetypeSegment::eSegType lttype = isSolid?ON_LinetypeSegment::eSegType::stLine : ON_LinetypeSegment::eSegType::stSpace;
    rc = pLinetype->SetSegment(index, length, lttype);
  }
  return rc;
}

RH_C_FUNCTION void ON_Linetype_GetSegment(const ON_Linetype* pConstLinetype, int index, double* length, bool* isSolid)
{
  if( pConstLinetype && length && isSolid )
  {
    ON_LinetypeSegment seg = pConstLinetype->Segment(index);
    *length = seg.m_length;
    *isSolid = seg.m_seg_type==ON_LinetypeSegment::eSegType::stLine;
  }
}

RH_C_FUNCTION bool ON_Linetype_SetSegments(ON_Linetype* pLinetype, int segment_count, /*ARRAY*/const double* pSegmentLengths)
{
  if (nullptr == pLinetype)
    return false;

  bool rc = false;
  if (segment_count < 2)
  {
    ON_SimpleArray<ON_LinetypeSegment> empty;
    rc = pLinetype->SetSegments(empty);
    pLinetype->AppendSegment(ON_LinetypeSegment::OneMillimeterLine);
  }

  if (segment_count > 0 && pSegmentLengths)
  {
    ON_SimpleArray<double> segment_lengths(segment_count);
    segment_lengths.Append(segment_count, pSegmentLengths);
    
    ON_SimpleArray<ON_LinetypeSegment> segments(segment_count);
    for (int i = 0; i < segment_count; i++)
    {
      ON_LinetypeSegment& seg = segments.AppendNew();
      seg.m_length = fabs(segment_lengths[i]);
      seg.m_seg_type = (segment_lengths[i] >= 0.0) 
        ? ON_LinetypeSegment::eSegType::stLine 
        : ON_LinetypeSegment::eSegType::stSpace;
    }

    rc = pLinetype->SetSegments(segments);
  }

  if (rc)
    rc = pLinetype->IsValid();
  return rc;
}

RH_C_FUNCTION double ON_Linetype_GetWidth(const ON_Linetype* linetype)
{
  return linetype ? linetype->Width() : 0;
}

RH_C_FUNCTION void ON_Linetype_SetWidth(ON_Linetype* linetype, double width)
{
  if (linetype)
    linetype->SetWidth(width);
}

RH_C_FUNCTION ON::LengthUnitSystem ON_Linetype_GetWidthUnits(const ON_Linetype* linetype)
{
  return linetype ? linetype->WidthUnits() : ON::LengthUnitSystem::None;
}

RH_C_FUNCTION void ON_Linetype_SetWidthUnits(ON_Linetype* linetype, ON::LengthUnitSystem units)
{
  if (linetype)
    linetype->SetWidthUnits(units);
}

RH_C_FUNCTION void ON_Linetype_GetTaperPoints(const ON_Linetype* linetype, ON_SimpleArray<ON_2dPoint>* points)
{
  if (linetype && points)
  {
    const ON_SimpleArray<ON_2dPoint>* ltpts = linetype->TaperPoints();
    if (ltpts)
      *points = *ltpts;
  }
}

RH_C_FUNCTION void ON_Linetype_SetTaper(ON_Linetype* linetype, double startwidth, ON_2DPOINT_STRUCT pt, double endWidth)
{
  if (linetype)
  {
    ON_2dPoint taperPoint(pt.val);
    if (taperPoint.IsValid())
      linetype->SetTaper(startwidth, taperPoint, endWidth);
    else
      linetype->SetTaper(startwidth, endWidth);
  }
}
RH_C_FUNCTION void ON_Linetype_RemoveTaper(ON_Linetype* linetype)
{
  if (linetype)
    linetype->RemoveTaper();
}
