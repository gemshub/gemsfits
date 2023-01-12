
#include <QJsonArray>
#include <QVector>
#include "v_json.h"
#include "verror.h"
#include "charts/graph_data.h"

namespace jsonui17 {


QColor colorAt(const QColor &start, const QColor &end, qreal pos)
{
    Q_ASSERT(pos >= 0.0 && pos <= 1.0);
    qreal r = start.redF() + ((end.redF() - start.redF()) * pos);
    qreal g = start.greenF() + ((end.greenF() - start.greenF()) * pos);
    qreal b = start.blueF() + ((end.blueF() - start.blueF()) * pos);
    QColor c;
    c.setRgbF(r, g, b);
    return c;
}

//---------------------------------------------------------------------------
// SeriesLineData
//---------------------------------------------------------------------------


void SeriesLineData::toBsonObject( bson *obj ) const
{
    bson_append_int( obj, "gpt", markerShape );
    bson_append_int( obj, "gps", markerSize );
    bson_append_int( obj, "gls", penSize );
    bson_append_int( obj, "glt", penStyle );
    bson_append_int( obj, "gsp", spline );
    bson_append_int( obj, "gndx", xcolumn );
    bson_append_int( obj, "grd", red );
    bson_append_int( obj, "ggr",  green );
    bson_append_int( obj, "gbl",  blue );
    bson_append_string( obj, "gnm",  name.c_str() );
}

void SeriesLineData::fromBsonObject( const char *obj )
{
    bson_find_value_def( obj, "gpt", markerShape, 0 );
    bson_find_value_def( obj, "gps", markerSize, 4 );
    bson_find_value_def( obj, "gls", penSize, 2 );
    bson_find_value_def( obj, "glt", penStyle, 0 );
    bson_find_value_def( obj, "gsp", spline, 0 );
    bson_find_value_def( obj, "gndx", xcolumn, -1 );
    bson_find_value_def( obj, "grd", red, 25 );
    bson_find_value_def( obj, "ggr", green, 0 );
    bson_find_value_def( obj, "gbl", blue, 150 );
    bson_find_string( obj, "gnm", name );
}


void SeriesLineData::toJsonObject(QJsonObject& json) const
{
    json["gpt"] = markerShape;
    json["gps"] = markerSize;
    json["gls"] = penSize;
    json["glt"] = penStyle;
    json["gsp"] = spline;
    json["gndx"] = xcolumn;
    json["grd"] = red;
    json["ggr"] = green;
    json["gbl"] =  blue;
    json["gnm"] =  name.c_str();
}

void SeriesLineData::fromJsonObject(const QJsonObject& json)
{
    markerShape = json[ "gpt" ].toInt( 0 );
    markerSize = json[ "gps" ].toInt( 4 );
    penSize = json[ "gls" ].toInt( 2 );
    penStyle = json[ "glt" ].toInt( 0 );
    spline = json[ "gsp" ].toInt( 0 );
    xcolumn = json[ "gndx" ].toInt( -1 );
    red = json[ "grd" ].toInt( 25 );
    green = json[ "ggr" ].toInt( 0 );
    blue = json[ "gbl" ].toInt( 150 );
    name = json["gnm"].toString("").toStdString();
}

//---------------------------------------------------------------------------
// ChartData
//---------------------------------------------------------------------------

void ChartData::setGraphType(int newtype)
{
    graphType = newtype;
    auto model_type = static_cast<GRAPHTYPES>(graphType);
    for( auto& model: modelsdata)
        model->setGraphType(model_type);
}

void ChartData::setMinMaxRegion( double reg[4] )
{
    region[0] = reg[0];
    region[1] = reg[1];
    region[2] = reg[2];
    region[3] = reg[3];
    part[0] = reg[0]+(reg[1]-reg[0])/3;
    part[1] = reg[1]-(reg[1]-reg[0])/3;
    part[2] = reg[2]+(reg[3]-reg[2])/3;
    part[3] = reg[3]-(reg[3]-reg[2])/3;

}

void ChartData::toBsonObject( bson *obj ) const
{
    int ii;
    char buf[100];

    bson_append_string( obj,"title", title.c_str() );
    bson_append_int( obj, "graphType", graphType );

    // define grid of plot
    bson_append_int( obj, "axisTypeX", axisTypeX );
    bson_append_int( obj, "axisTypeY", axisTypeY );
    bson_append_string( obj,"xName", xName.c_str() );
    bson_append_string( obj,"yName", yName.c_str() );
    bson_append_string( obj, "axisFont", axisFont.toString().toStdString().c_str() );

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
          for(size_t ii=0; ii<linesdata.size(); ii++)
          {
              bson_append_start_object( obj, std::to_string(ii).c_str());
              linesdata[ii].toBsonObject( obj );
              bson_append_finish_object( obj );
          }
      bson_append_finish_array(obj);

      // data to isoline plots
      bson_append_start_array(obj, "scale");
           for(size_t ii=0; ii<modelsdata.size(); ii++)
           {
               bson_append_start_object( obj, std::to_string(ii).c_str());
               modelsdata[ii]->toBsonObject( obj );
               bson_append_finish_object( obj );
           }
       bson_append_finish_array(obj);
}

void ChartData::fromBsonObject( const char *obj )
{
    size_t ii;
    std::vector<double> reg_part;
    std::vector<int> b_col_vec;

    if( !bson_find_string( obj, "title", title ) )
        title = "title";
    bson_find_value_def<int>( obj, "graphType", graphType, LineChart );

    // define grid of plot
    bson_find_value_def( obj, "axisTypeX", axisTypeX, 5 );
    bson_find_value_def( obj, "axisTypeY", axisTypeY, 5 );

    std::string fntName;
    if( bson_find_string( obj, "axisFont", fntName ) )
        axisFont.fromString( fntName.c_str() );

    if( !bson_find_string( obj, "xName", xName ) )
        xName = "x";
    if( !bson_find_string( obj, "yName", yName ) )
        yName = "y";

    auto arr  = bson_find_array(  obj, "region" );
    for( ii=0; ii<4; ii++)
    {
        if(!bson_find_value( arr, std::to_string(ii).c_str(), region[ii] ) )
            region[ii] = 0;
    }

    arr  = bson_find_array(  obj, "part" );
    for( ii=0; ii<4; ii++)
    {
        if(!bson_find_value( arr,  std::to_string(ii).c_str(), part[ii] ) )
            part[ii] = 0;
    }

    arr  = bson_find_array(  obj, "b_color" );
    for( ii=0; ii<3; ii++)
    {
        if(!bson_find_value( arr, std::to_string(ii).c_str(), b_color[ii] ) )
            b_color[ii] = 255;
    }

    linesdata.clear();
    SeriesLineData linebuf;
    arr  = bson_find_array(  obj, "lines" );
    bson_iterator iter;
    bson_iterator_from_buffer(&iter, arr /*bson_iterator_value(it)*/);
    while (bson_iterator_next(&iter))
    {
        linebuf.fromBsonObject( bson_iterator_value(&iter) );
        linesdata.push_back( linebuf );
    }

    arr  = bson_find_array(  obj, "models" );
    bson_iterator iter_mod;
    bson_iterator_from_buffer(&iter_mod, arr /*bson_iterator_value(it)*/);
    ii=0;
    while (bson_iterator_next(&iter_mod))
    {
        if( ii >= modelsdata.size())
            break;
        modelsdata[ii++]->fromBsonObject( bson_iterator_value(&iter_mod) );
    }

    // refresh model type
    setGraphType(graphType);
}

void ChartData::toJsonObject(QJsonObject& json) const
{
    json[ "title" ] =  title.c_str();
    json[ "graphType" ] = graphType;
    json[ "axisTypeX" ] = axisTypeX;
    json[ "axisTypeY" ] = axisTypeY;
    json[ "axisFont" ] = axisFont.toString();
    json[ "xName" ] =  xName.c_str();
    json[ "yName" ] = yName.c_str();

    QJsonArray regArray;
    QJsonArray partArray;
    for(size_t ii=0; ii<4; ii++) {
        regArray.append(region[ii]);
        partArray.append(part[ii]);
    }
    json["region"] = regArray;
    json["part"] = partArray;

    QJsonArray colorArray;
    for(size_t ii=0; ii<3; ii++)
        colorArray.append(b_color[ii]);
    json["b_color"] = colorArray;

    QJsonArray linesArray;
    for(size_t ii=0; ii<linesdata.size(); ii++)
    {
        QJsonObject lnObject;
        linesdata[ii].toJsonObject(lnObject);
        linesArray.append(lnObject);
    }
    json["lines"] = linesArray;

    QJsonArray modelArray;
    for(size_t ii=0; ii<modelsdata.size(); ii++)
    {
        QJsonObject lnObject;
        modelsdata[ii]->toJsonObject(lnObject);
        modelArray.append(lnObject);
    }
    json["models"] = modelArray;
}

void ChartData::fromJsonObject(const QJsonObject& json)
{
    title = json[ "title" ].toString( "Graph title" ).toStdString();
    graphType = json[ "graphType" ].toInt( LineChart );
    axisTypeX = json[ "axisTypeX" ].toInt( 5 );
    axisTypeY = json[ "axisTypeY" ].toInt( 5 );
    QString fntname = json[ "axisFont" ].toString( "Sans Serif" );
    axisFont.fromString( fntname );
    xName = json[ "xName" ].toString( "x" ).toStdString();
    yName = json[ "yName" ].toString( "y" ).toStdString();

    QJsonArray regArray = json["region"].toArray();
    QJsonArray partArray = json["part"].toArray();
    if( regArray.size() > 3 && partArray.size() > 3 )
        for(int ii=0; ii<4; ii++)
        {
            region[ii] = regArray[ii].toDouble();
            part[ii] = partArray[ii].toDouble();
        }

    QJsonArray colorArray = json["b_color"].toArray();
    if( colorArray.size() > 2 )
        for(int ii=0; ii<3; ii++)
            b_color[ii] = colorArray[ii].toInt();

    linesdata.clear();
    SeriesLineData linebuf;
    QJsonArray linesArray = json["lines"].toArray();
    for(int ii=0; ii<linesArray.size(); ii++)
    {
        QJsonObject lnObject = linesArray[ii].toObject();
        linebuf.fromJsonObject(lnObject);
        linesdata.push_back(  linebuf );
    }

    linesArray = json["models"].toArray();
    for(int ii=0; ii<linesArray.size(); ii++)
    {
        if( ii >= static_cast<int>(modelsdata.size()) )
            break;
        QJsonObject lnObject = linesArray[ii].toObject();
        modelsdata[ii]->fromJsonObject(lnObject);
    }
    // refresh model type
    setGraphType(graphType);
}

void ChartData::updateXSelections()
{
    size_t defined_lines = linesdata.size();
    size_t nLines = 0;

    for( size_t ii=0; ii<modelsdata.size(); ii++)
    {
        auto numxColms = modelsdata[ii]->getAbscissaNumber();
        auto nLinN =  modelsdata[ii]->getSeriesNumber();
        for( size_t jj=0; jj<nLinN; jj++, nLines++ )
        {
            if( nLines >= defined_lines )
                Error( "ChartData", "error into graph data.."  );
            if( linesdata[nLines].getXColumn() >= numxColms )
                linesdata[nLines].setXColumn(-1);
            if( linesdata[nLines].getXColumn() == -1 && numxColms>=1 ) {
                linesdata[nLines].setXColumn(0);
            }

        }
    }
}

void ChartData::updateYSelections( bool updateNames )
{
    size_t defined_lines = linesdata.size();
    size_t nLines = 0;

    for( size_t ii=0; ii<modelsdata.size(); ii++)
    {
        auto nLinN =  modelsdata[ii]->getSeriesNumber();
        for( size_t jj=0; jj<nLinN; jj++, nLines++ )
        {
            if( nLines >= defined_lines ) {
                linesdata.push_back( SeriesLineData( jj, nLinN, modelsdata[ii]->getName(nLines)  ) );
                if( modelsdata[ii]->getAbscissaNumber() >= 1 )
                    linesdata[nLines].setXColumn(0);
            }
            if( updateNames )
                linesdata[nLines].setName( modelsdata[ii]->getName(nLines) );
        }
    }
    linesdata.resize(nLines);
}



} // namespace jsonui17

//--------------------- End of graph_data.cpp ---------------------------
