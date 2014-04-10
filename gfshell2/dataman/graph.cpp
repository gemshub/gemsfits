//-------------------------------------------------------------------
// $Id: graph.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TPlotLine, TPlot, GraphData and GraphWindow classes
// (Plotting system description)
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <cstdio>
#include "GraphDialog.h"
#include "FITMainWindow.h"
//#include "gdatastream.h"
//#include "v_ejdb.h"
//#include "v_object.h"

//---------------------------------------------------------------------------
// TPlotLine
//---------------------------------------------------------------------------

void TPlotLine::toBsonObject( bson *obj )
{
   bson_append_int( obj, "gtp", type );
   bson_append_int( obj, "gsz", sizes );
   bson_append_int( obj, "gndx", ndxX );
   bson_append_int( obj, "grd", red );
   bson_append_int( obj, "ggr",  green );
   bson_append_int( obj, "gbl",  blue );
   bson_append_string( obj, "gnm",  name );
}

void TPlotLine::fromBsonObject( const char *obj )
{
    if(!bson_find_value( obj, "gtp", type ) )
        Error( "E011BSon: ", "Undefined gtp.");
    if(!bson_find_value( obj, "gsz", sizes ) )
        Error( "E012BSon: ", "Undefined gsz.");
    if(!bson_find_value( obj, "gndx", ndxX ) )
        Error( "E013BSon: ", "Undefined gndx.");
    if(!bson_find_value( obj, "grd", red ) )
        Error( "E014BSon: ", "Undefined grd.");
    if(!bson_find_value( obj, "ggr", green ) )
        Error( "E01501BSon: ", "Undefined ggr.");
    if(!bson_find_value( obj, "gbl", blue ) )
        Error( "E016BSon: ", "Undefined gbl.");
    string valStr;
    bson_find_string( obj, "gnm", valStr );
    memcpy( name, valStr.c_str(), 15);
}

//---------------------------------------------------------------------------
// TPlot
//---------------------------------------------------------------------------

TPlot::TPlot( QSortFilterProxyModel *aModel, int lines, const vector<int>& xval,
              const vector<int>& yval, const vector<string>& aynames ):
     pModel(aModel), first(0), ynames(aynames), xcolms(xval), ycolms(yval), dX(lines)
{
  //  ((TMatrixModel *)pModel->sourceModel())->getXYvectors( dX, xcolms, ycolms,  ynames);

   // put graph by column
    dY1 = ycolms.size();
    nAbs = xcolms.size();
    ErrorIf( dY1== 0, "Graphics", "You fogot select an ordinate" );
}

TPlot::TPlot( const TPlot& plt, int aFirst ):
       pModel(plt.pModel), xcolms(plt.xcolms), ycolms(plt.ycolms), ynames(plt.ynames), first(aFirst)
{
    dX =plt.dX;
    nAbs = plt.nAbs;
    dY1 =plt.dY1;
}

TPlot::~TPlot()
{}

const string& TPlot::getName( int ii )
{
    return ynames[ii];
}

// get point to draw one line
QPointF TPlot::getPoint( int line, int number, int ndxAbs )
{
    double x, y;

    if(ndxAbs >= nAbs )
        ndxAbs = 0;

    if( ndxAbs < 0 )
        return QPointF( DOUBLE_EMPTY, DOUBLE_EMPTY );

    QModelIndex wIndex = 	pModel->index( number, ycolms[ line ] );
     y = wIndex.data(Qt::EditRole).toDouble();

    // put graph by column
      if( nAbs <= 0 )
            x = number;
      else
            x =  wIndex.sibling( number, xcolms[ ndxAbs ] ).data(Qt::EditRole).toDouble();

    return QPointF( x, y);
}

// Build array of points to draw one line
// return number of points in line
int TPlot::getPointLine( int j, QVector<QPointF>& points, int ndxAbs )
{
    QPointF pos;
    points.clear();

    for(int  ii=0; ii<dX; ii++ )
    {   pos = getPoint( j, ii, ndxAbs );
        //if( pos.x() == DOUBLE_EMPTY || pos.y() == DOUBLE_EMPTY )
        //  continue;
        points.append(pos);
    }
    return dX;
}

// Build array of points to draw one line
// return number of points in line
int TPlot::getPointTube( int line, QVector<QwtIntervalSample>& points, int ndxAbs )
{
    QPointF pos;
    double miny, maxy;

    for(int  ii=0; ii<dX; ii++ )
    {
        pos = getPoint( line, ii, ndxAbs );
        miny = points[ii].interval.maxValue();

        if( pos.x() == DOUBLE_EMPTY || pos.y() == DOUBLE_EMPTY )
          maxy = miny;
        else
          maxy = miny+pos.y();
        points[ii].interval.setMinValue( miny );
        points[ii].interval.setMaxValue( maxy );
    }
    return dX;
}


// Find min and max values x,y for one curve line
void TPlot::getMaxMinLine( QPointF& min, QPointF& max, int line, int ndxAbs )
{
    QPointF point;
    int jj = line;

    min = getPoint( jj, 0, ndxAbs );
    max = min;
    for( int ii =0; ii<dX; ii++)
    {
           point = getPoint( jj, ii, ndxAbs );
           if( point.x() == DOUBLE_EMPTY || point.y() == DOUBLE_EMPTY )
             continue;
           if( min.x() > point.x() || min.x() == DOUBLE_EMPTY  )
               min.setX( point.x() );
           if( max.x() < point.x() || max.x() == DOUBLE_EMPTY )
               max.setX( point.x() );
           if( min.y() > point.y() || min.y() == DOUBLE_EMPTY )
                min.setY( point.y() );
           if( max.y() < point.y() || max.y() == DOUBLE_EMPTY )
               max.setY( point.y() );
    }
}

bool operator ==( const TPlot& iEl,  const TPlot& iEr)
{
    if( iEl.xcolms.size() != iEr.xcolms.size() ||
          iEl.ycolms.size() != iEr.ycolms.size()  )
       return false;
    return ( equal( iEl.xcolms.begin(), iEl.xcolms.end(), iEr.xcolms.begin()) &&
             equal( iEl.ycolms.begin(), iEl.ycolms.end(), iEr.ycolms.begin()) );
}

//---------------------------------------------------------------------------
// GraphData
//---------------------------------------------------------------------------

void GraphData::toBsonObject( bson *obj )
{
    int ii;
    char buf[100];

    // !!!!  vector<TPlot> plots;    loaded/unloaded separately

    bson_append_string( obj,"title", title.c_str() );
    bson_append_int( obj, "graphType", graphType );

    // define grid of plot
    bson_append_int( obj, "axisTypeX", axisTypeX );
    bson_append_int( obj, "axisTypeY", axisTypeY );
    bson_append_string( obj,"xName", xName.c_str() );
    bson_append_string( obj,"yName", yName.c_str() );

    bson_append_start_array(obj, "region");
    for( ii=0; ii<4; ii++)
    {
        sprintf(buf, "%d", ii);
        bson_append_double( obj, buf, region[ii] );
    }
    bson_append_finish_array(obj);

    bson_append_start_array(obj, "part");
    for( ii=0; ii<4; ii++)
    {
        sprintf(buf, "%d", ii);
        bson_append_double( obj, buf, part[ii] );
    }
    bson_append_finish_array(obj);

    bson_append_start_array(obj, "b_color");
    for( ii=0; ii<3; ii++)
    {
        sprintf(buf, "%d", ii);
        bson_append_int( obj, buf, b_color[ii] );
    }
    bson_append_finish_array(obj);

     // define curves
     bson_append_start_array(obj, "lines");
          for(int ii=0; ii<lines.size(); ii++)
          {
              sprintf(buf, "%d", ii);
              bson_append_start_object( obj, buf);
              lines[ii].toBsonObject( obj );
              bson_append_finish_object( obj );
          }
      bson_append_finish_array(obj);

      // data to isoline plots
      bson_append_start_array(obj, "scale");
           for(int ii=0; ii<scale.size(); ii++)
           {
               sprintf(buf, "%d", ii);
               bson_append_start_object( obj, buf);
               bson_append_int( obj, "grd", scale[ii].red() );
               bson_append_int( obj, "ggr",  scale[ii].green());
               bson_append_int( obj, "gbl", scale[ii].blue() );
               bson_append_finish_object( obj );
           }
       bson_append_finish_array(obj);
}

void GraphData::fromBsonObject( const char *obj )
{
    int ii;
    char buf[100];
   const char *arr;

   if( !bson_find_string( obj, "title", title ) )
        title = "title";
    if(!bson_find_value( obj, "graphType", graphType ) )
        graphType = LINES_POINTS;

    // define grid of plot
    if(!bson_find_value( obj, "axisTypeX", axisTypeX ) )
        axisTypeX = 4;
    if(!bson_find_value( obj, "axisTypeY", axisTypeY ) )
        axisTypeY = 4;

    if( !bson_find_string( obj, "xName", xName ) )
        xName = "x";
    if( !bson_find_string( obj, "yName", yName ) )
        yName = "y";

    arr  = bson_find_array(  obj, "region" );
    for( ii=0; ii<4; ii++)
    {
        sprintf(buf, "%d", ii);
        if(!bson_find_value( arr,  buf, region[ii] ) )
            region[ii] = 0;
    }

    arr  = bson_find_array(  obj, "part" );
    for( ii=0; ii<4; ii++)
    {
        sprintf(buf, "%d", ii);
        if(!bson_find_value( arr,  buf, part[ii] ) )
            part[ii] = 0;
    }

    arr  = bson_find_array(  obj, "b_color" );
    for( ii=0; ii<3; ii++)
    {
        sprintf(buf, "%d", ii);
        if(!bson_find_value( arr,  buf, b_color[ii] ) )
            b_color[ii] = 255;
    }

    lines.clear();
    TPlotLine linebuf;
    //bson_iterator it;
    //bson_type type;
    //type =  bson_find_from_buffer(&it, obj, "lines" );
    //ErrorIf( type != BSON_ARRAY, "E005BSon: ", "Must be array.");
    arr  = bson_find_array(  obj, "lines" );
    bson_iterator iter;
    bson_iterator_from_buffer(&iter, arr /*bson_iterator_value(it)*/);
    while (bson_iterator_next(&iter))
    {
        linebuf.fromBsonObject( bson_iterator_value(&iter) );
        lines.push_back( linebuf );
    }

    scale.clear();
    int red, green, blue;
    arr  = bson_find_array(  obj, "scale" );
    //bson_iterator iter;
    bson_iterator_from_buffer(&iter, arr );
    while (bson_iterator_next(&iter))
    {

        if(!bson_find_value( bson_iterator_value(&iter), "grd", red ) )
            red = 256;
        if(!bson_find_value( bson_iterator_value(&iter), "ggr", green ) )
            green = 256;
        if(!bson_find_value( bson_iterator_value(&iter), "gbl", blue ) )
            blue = 256;
        scale.push_back( QColor( red, green, blue ) );
    }
}

/* The constructor
GraphData::GraphData( const GraphData& data ):
        title(data.title), axisTypeX(data.axisTypeX), axisTypeY(data.axisTypeY),
        graphType(data.graphType), xName(data.xName), yName(data.yName)
        //isBackgr_color(data.isBackgr_color)
{
    int ii;
    int jj, nLines;

    // Insert Plots and curves description
    plots.clear();
    lines.clear();
    for( ii=0, nLines=0; ii<data.plots.size(); ii++)
    {
        plots.push_back( TPlot(data.plots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
             lines.push_back( TPlotLine( data.lines[nLines] ) );
    }

    for(int i=0; i<4; i++ )
    {
        region[i] = data.region[i];
        part[i]   = data.part[i];
    }

    b_color[0] = data.b_color[0];
    b_color[1] = data.b_color[1];
    b_color[2] = data.b_color[2];

    if( graphType == ISOLINES )
    {
         goodIsolineStructure( graphType );
         scale.clear();
         for( ii=0; ii<data.scale.size(); ii++)
            scale.push_back( QColor( data.scale[ii] ));
    }
 }
 */

/// The constructor
GraphData::GraphData( const vector<TPlot>&  aPlots, const char * aTitle,
               const char *aXName, const char *aYName, int agraphType ):
        title(aTitle), axisTypeX(4), axisTypeY(4), graphType( agraphType )
{
    int ii;
    int jj, nLines, ndxAbs;

    xName = string( aXName, 0, 9);
    yName = string( aYName, 0, 9);

    // Insert Plots and curves description
    plots.clear();
    lines.clear();
    for( ii=0, nLines=0; ii<aPlots.size(); ii++)
    {
        plots.push_back( TPlot(aPlots[ii], nLines ));
        int nLinN = plots[ii].getLinesNumber();
        for( jj=0; jj<nLinN; jj++, nLines++ )
        {
           if( jj < plots[ii].getNAbs() )
             ndxAbs = jj;
          else  ndxAbs = 0;
          lines.push_back( TPlotLine( jj, nLinN, plots[ii].getName(jj).c_str(), 0, 4, 0, ndxAbs  ) );
        }
    }

   resetMinMaxRegion();

   QColor color(Qt::white);
   b_color[0] = color.red();
   b_color[1] = color.green();
   b_color[2] = color.blue();

}

GraphData::~GraphData()
{}

/// Change plot lines selection
void GraphData::setNewPlot( const vector<TPlot>& aPlots )
{
    int ii;
    int jj, nLines, ndxAbs;

    // test change a Plots
    if( !(plots.size() != aPlots.size() ) &&
            equal( plots.begin(), plots.end(), aPlots.begin()) )
    {
        cout << "Graphics settings did not change ..." << endl;
        return; // plot settings did not change
    }
    cout << "!!! Plots changed" << endl;

    plots.clear();
    int defined_lines = lines.size();
    for( ii=0, nLines=0; ii<aPlots.size(); ii++)
    {
        plots.push_back( TPlot(aPlots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
        {
            if( jj < plots[ii].getNAbs() )
              ndxAbs = jj;
           else  ndxAbs = 0;

           if( nLines >= defined_lines )
              lines.push_back( TPlotLine( nLines, plots[ii].getLinesNumber(), plots[ii].getName(jj).c_str(), 0, 4, 0, ndxAbs  ) );
           else // change line name
           {   lines[nLines].setName( plots[ii].getName(jj).c_str());
               lines[nLines].setIndex( ndxAbs );
           }
        }
    }
    if( defined_lines > nLines )
      lines.resize(nLines);
    resetMinMaxRegion();
}

void GraphData::resetMinMaxRegion()
{
    int ii, jj, nLines;
    double minX, maxX, minY, maxY;
    QPointF min, max;

    if( plots.size() == 0 )
      return;

    if( graphType == ISOLINES )
    {
       goodIsolineStructure( graphType );
       //plots[0].getMaxMinIso(  min, max );
       //       minX = min.x();
       //       maxX = max.x();
       plots[0].getMaxMinLine( min, max, 0, 0 );
       minX = min.y();
       maxX = max.y();
       plots[0].getMaxMinLine( min, max, 1, 0 );
       minY = min.y();
       maxY = max.y();
       setColorList();
       setScales();
    }
    else
    { // set default min-max region
      plots[0].getMaxMinLine(  min, max, 0, getIndex(0) );
      minX = min.x();
      maxX = max.x();
      minY = min.y();
      maxY = max.y();

      for( ii=0, nLines=0; ii<plots.size(); ii++)
       for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
       {  plots[ii].getMaxMinLine( min, max, jj, getIndex(nLines) );
         if( minX > min.x() ) minX = min.x();
         if( maxX < max.x() ) maxX = max.x();
         if( minY > min.y() ) minY = min.y();
         if( maxY < max.y() ) maxY = max.y();
       }
    }

   if( maxX == minX ) maxX += 1.;
   if( maxY == minY ) maxY += 1.;

   adjustAxis(minX, maxX, axisTypeX);
   adjustAxis(minY, maxY, axisTypeY);

   region[0] = minX;
   region[1] = maxX;
   region[2] = minY;
   region[3] = maxY;
   part[0] = minX+(maxX-minX)/3;
   part[1] = maxX-(maxX-minX)/3;
   part[2] = minY+(maxY-minY)/3;
   part[3] = maxY-(maxY-minY)/3;

}



void GraphData::getIndexes( QVector<int>& first, QVector<int>& maxXndx )
{
    for(int ii=0; ii<plots.size(); ii++)
    {
        first.append( plots[ii].getFirstLine() );
        maxXndx.append( plots[ii].getNAbs() );
    }

}

void  GraphData::adjustAxis( double& min, double& max, int& numTicks)
{
    const int minTicks = 4;
    double grossStep = (max-min)/minTicks;
    double step = pow(10. , floor(log10(grossStep)));

    if(5*step < grossStep )
    {
        step *=5;
    }
    else if(2*step < grossStep )
           step *=2;

    numTicks = int(ceil(max/step)-floor(min/step));
    if( numTicks < minTicks)
       numTicks = minTicks;
    min = floor(min/step)*step;
    max = ceil(max/step)*step;
}

// must be changed -------------------------------------------

bool GraphData::goodIsolineStructure( int aGraphType )
{
  if( aGraphType == ISOLINES &&
      !(plots.size() >= 2 &&
        plots[0].getLinesNumber() >= 3 &&
        plots[1].getLinesNumber() >= 5
        ) )
   {
     string str = "Invalid sizes of graphic arrays:";
     /*if(plots.size() >= 1)
             str += aObj[ plots[0].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     str += " and ";
     if(plots.size() >= 2)
             str += aObj[ plots[1].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     */
     Error( " ISOLINES graphic", str );
    }
  return true;
}

// setup color scale for ISOLINE graphics
// get data from plots[1] object, colums 2, 3, 4
void GraphData::setColorList()
{
  ///int cred, cgreen, cblue;
  ///int nObjY = plots[1].getObjY();
  scale.clear();

  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     ///cred = (int)aObj[nObjY].Get(ii,2);
     ///cgreen = (int)aObj[nObjY].Get(ii,3);
     ///cblue = (int)aObj[nObjY].Get(ii,4);

     ///if( ( cred ==0 && cgreen == 0 && cblue == 0 ) ||
     ///    cred < 0 || cgreen < 0 || cblue < 0 ||
     ///    cred > 255 || cgreen > 255 || cblue > 255 )
     { //default
         if(!ii)
            scale.push_back( QColor( Qt::red ) );
         else if( ii == plots[1].getdX()-1 )
               scale.push_back( QColor( Qt::darkGreen ) );
              else
                scale.push_back( QColor( Qt::yellow ) );
     }
     ///else
     ///  scale.push_back( QColor( cred, cgreen, cblue ));
  }

}

double GraphData::getValueIsoline(int ii)
{
    ///return aObj[plots[1].getObjY()].Get(ii,0);
}

void GraphData::setValueIsoline(double val, int ii)
{
   /// aObj[plots[1].getObjY()].Put( val, ii,0);
}

// put color scale for ISOLINE graphics to object plots[1]
//  colums 2, 3, 4
void GraphData::getColorList()
{
  /** int nObjY = plots[1].getObjY();
  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     if( ii >= (int)scale.size() )
       break;
     aObj[nObjY].Put( scale[ii].red(), ii, 2 );
     aObj[nObjY].Put( scale[ii].green(), ii, 3 );
     aObj[nObjY].Put( scale[ii].blue(),  ii, 4 );
  }
  */
}

// put sizes scale for ISOLINE graphics to object plots[1]
//  colum 0
void GraphData::setScales()
{
  /** int nObjY = plots[1].getObjY();
  double delta = 1. / (plots[1].getdX()-1);

  for( int ii=0; ii< plots[1].getdX()-1; ii++ )
  {
    if( aObj[nObjY].Get(ii,0) == 0 )
     aObj[nObjY].Put( 1.-delta*(ii), ii, 0 );
  }
  aObj[nObjY].Put( 0., plots[1].getdX()-1, 0 );
  */
}

//--------------------- End of graph.cpp ---------------------------
