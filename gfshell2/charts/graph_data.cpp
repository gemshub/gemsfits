
#include <QJsonArray>
#include <QVector>
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


jsonio::JsonFree SeriesLineData::toBsonObject() const
{
    jsonio::JsonFree object;
    object["gpt"] = markerShape;
    object["gps"] = markerSize;
    object["gls"] = penSize;
    object["glt"] = penStyle;
    object["gsp"] = spline;
    object["gndx"] = xcolumn;
    object["grd"] = red;
    object["ggr"] = green;
    object["gbl"] =  blue;
    object["gnm"] =  name;
    return object;
}

void SeriesLineData::fromBsonObject( const jsonio::JsonFree& object )
{
    markerShape = object.value("gpt", 0);
    markerSize = object.value("gps", 4);
    penSize = object.value("gls", 2);
    penStyle = object.value("glt", 0);
    spline = object.value("gsp", 0);
    xcolumn = object.value("gndx", -1);
    red = object.value("grd", 25);
    green = object.value("ggr", 0);
    blue = object.value("gbl", 150);
    name = object.value("gnm", "");
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

jsonio::JsonFree ChartData::toBsonObject() const
{
    jsonio::JsonFree object;
    object[ "title" ] =  title;
    object[ "graphType" ] = graphType;
    object[ "axisTypeX" ] = axisTypeX;
    object[ "axisTypeY" ] = axisTypeY;
    object[ "axisFont" ] = axisFont.toString().toStdString();
    object[ "xName" ] =  xName;
    object[ "yName" ] = yName;

    object["region"] = region;
    object["part"] = part;
    object["b_color"] = b_color;

    auto linesArray = jsonio::JsonFree::array();
    for(const auto& line: linesdata) {
        linesArray.push_back(line.toBsonObject());
    }
    object["lines"] = linesArray;

    auto modelArray = jsonio::JsonFree::array();
    for(const auto& amodel: modelsdata) {
        modelArray.push_back(amodel->toBsonObject());
    }
    object["models"] = modelArray;

    return object;
}

void ChartData::fromBsonObject( const jsonio::JsonFree& object )
{
    title = object.value("title", "Graph title");
    graphType = object.value<int>("graphType", LineChart);
    axisTypeX = object.value("axisTypeX", 5);
    axisTypeY = object.value("axisTypeY", 5);
    QString fntname = object.value("axisFont", "Sans Serif").c_str();
    axisFont.fromString( fntname );
    xName = object.value("xName", "x");
    yName = object.value("yName", "y");

    auto arr = object.value<std::vector<double>>("region", std::vector<double>{0,0,0,0} );
    std::copy_n(arr.begin(), 4, region.begin());
    arr = object.value("part", std::vector<double>{0,0,0,0} );
    std::copy_n(arr.begin(), 4, part.begin());
    auto iarr = object.value("region", std::vector<int>{});
    std::copy_n(iarr.begin(), 3, b_color.begin());

    linesdata.clear();
    if(object.contains("lines")) {
        SeriesLineData linebuf;
        for(const auto& obj: object["lines"]) {
            linebuf.fromBsonObject(*obj);
            linesdata.push_back(linebuf);
        }
    }

    if(object.contains("models")) {
        size_t ii=0;
        for(const auto& obj: object["models"]) {
            if( ii >= modelsdata.size() )
                break;
            modelsdata[ii++]->fromBsonObject(*obj);
        }
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
