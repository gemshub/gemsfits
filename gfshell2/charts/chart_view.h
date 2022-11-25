#pragma once

#include <memory>
#include <QtCharts/QChartView>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QT_CHARTS_USE_NAMESPACE
#endif

namespace jsonui17 {

class PlotChartViewPrivate;
class ChartData;

/// Manages the graphical representation of the chart's series and axes
class PlotChartView : public QChartView
{
    Q_OBJECT

public slots:

    void updateLine( size_t line );
    void updateAll();
    void updateLines();
    void highlightLine( size_t line, bool enable  );
    void updateFragment( bool is_fragment );

signals:
    void fragmentChanged(QRectF  rect);

public:

    explicit PlotChartView( ChartData *graphdata, QWidget *parent = nullptr );
    ~PlotChartView();

    /**
      Render graphical representation of the chart's series and axes to a file

      Supported formats are:

      - pdf\n
        Portable Document Format PDF
      - ps\n
        Postcript
      - svg\n
        Scalable Vector Graphics SVG
      - all image formats supported by Qt\n
        see QImageWriter::supportedImageFormats()

      \param document title
      \param file_name Path of the file, where the document will be stored

    */
    void renderDocument( const QString &title, const QString &file_name );

    /// Switch to/from fragment mode
    void resetFragment( bool is_fragment );

protected:

    PlotChartViewPrivate* pdata;
    QRubberBand *rubberBand;

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

};

} // namespace jsonui17

