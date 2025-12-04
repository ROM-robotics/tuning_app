#ifndef ROM_TUNING_APP_DESIGN_H
#define ROM_TUNING_APP_DESIGN_H

#pragma once
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QWidget>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

#include <QtWidgets>
#include <QtCharts>
#include <QtMath>

namespace rom_dynamics::ui::qt {

class RobotVelocityGraph : public QWidget {
    Q_OBJECT

public:
    RobotVelocityGraph(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setStyleSheet("background-color: rgba(255,255,255,0);");

        chart_ = new QChart();
        series_ = new QLineSeries();

        axisX_ = new QValueAxis();
        axisY_ = new QValueAxis();

        chart_->addSeries(series_);
        chart_->addAxis(axisX_, Qt::AlignBottom);
        chart_->addAxis(axisY_, Qt::AlignLeft);

        series_->attachAxis(axisX_);
        series_->attachAxis(axisY_);

        chart_->legend()->hide();

        // Set chart background to transparent
        chart_->setBackgroundBrush(Qt::NoBrush);
        chart_->setBackgroundRoundness(0);

        // margin
        chart_->setMargins(QMargins(8, 8, 8, 8));

        // Set line color to green/yellow
        QPen linePen(QColor(180, 255, 80), 2);
        series_->setPen(linePen);

        // Set axis label color to white
        QColor axisColor(189, 183, 183, 80); 
        axisX_->setGridLineColor(axisColor);
        axisY_->setGridLineColor(axisColor);
        axisX_->setLabelsColor(axisColor);
        axisY_->setLabelsColor(axisColor);
        axisX_->setLinePen(QPen(axisColor));
        axisY_->setLinePen(QPen(axisColor));

            // Set font style for chart title and axis labels
        QFont titleFont;
        QColor titleColor("#03fc84");
        titleFont.setFamily("SF Pro Text");
        titleFont.setPointSize(10);

        chart_->setTitle("Odom, diff Odom Positions");
        
        chart_->setTitleBrush(QBrush(titleColor));
        chart_->setTitleFont(titleFont);

        QFont axisFont;
        axisFont.setFamily("SF Pro Text");
        axisFont.setPointSize(7); // smaller font size for axis
        axisX_->setLabelsFont(axisFont);
        axisY_->setLabelsFont(axisFont);

        // Set axis ranges and ticks
        axisX_->setRange(0, 8); // wider range, more grid lines
        axisX_->setTickCount(9); // more ticks
        axisY_->setRange(0, 8);
        axisY_->setTickCount(9);

        QVBoxLayout* layout = new QVBoxLayout(this);
        QChartView* chartView = new QChartView(chart_);
        chartView->setRenderHint(QPainter::Antialiasing);
        layout->addWidget(chartView);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void updateGraph(const QVector<double>& velocities)
    {
        series_->clear();

        if (velocities.isEmpty()) 
        {
            axisX_->setRange(0, 1);
            axisY_->setRange(0, 1);
            return;
        }
        double vMin = velocities[0];
        double vMax = velocities[0];
        for (int i = 0; i < velocities.size(); ++i) {
            double v = velocities[i];
            if (v < vMin) vMin = v;
            if (v > vMax) vMax = v;
            series_->append(i, v);
        }
        axisX_->setRange(0, velocities.size() > 1 ? velocities.size()-1 : 1);

        // If all values identical, expand a small band
        if (qFuzzyCompare(vMin, vMax)) {
            double pad = (qFabs(vMin) < 1e-6) ? 1.0 : qFabs(vMin) * 0.2;
            vMin -= pad;
            vMax += pad;
        }

        // Provide padding (10%)
        double span = vMax - vMin;
        double pad = span * 0.1;
        vMin -= pad;
        vMax += pad;

        // If data crosses zero, optionally ensure zero is visible with slight padding
        if (vMin > 0 && vMin < 0.05) vMin = 0; // snap near zero
        if (vMax < 0 && vMax > -0.05) vMax = 0;

        axisY_->setRange(vMin, vMax);
    }
    virtual ~RobotVelocityGraph() {}

private:
    QChart *chart_ = nullptr;
    QLineSeries *series_ = nullptr;
    QValueAxis *axisX_ = nullptr;
    QValueAxis *axisY_ = nullptr;
};


class RomPositionGraph : public QWidget {
    Q_OBJECT

public:
    const int MAX_POINTS = 20;
    RomPositionGraph(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setStyleSheet("background-color: rgba(255,255,255,0);");
        
        chart_ = new QChart();

        odomSeries_ = new QLineSeries();
        odomScatterSeries_ = new QScatterSeries();

        diffOdomSeries_ = new QLineSeries();
        diffOdomScatterSeries_ = new QScatterSeries();

        axisX_ = new QValueAxis();
        axisY_ = new QValueAxis();

        chart_->addSeries(odomSeries_);
        chart_->addSeries(odomScatterSeries_); 
        chart_->addSeries(diffOdomSeries_); 
        chart_->addSeries(diffOdomScatterSeries_); 

        chart_->addAxis(axisX_, Qt::AlignBottom);
        chart_->addAxis(axisY_, Qt::AlignLeft);

        odomSeries_->attachAxis(axisX_);
        odomSeries_->attachAxis(axisY_);
        diffOdomSeries_->attachAxis(axisX_);
        diffOdomSeries_->attachAxis(axisY_);

        odomScatterSeries_->attachAxis(axisX_);
        odomScatterSeries_->attachAxis(axisY_);
        diffOdomScatterSeries_->attachAxis(axisX_);
        diffOdomScatterSeries_->attachAxis(axisY_);

        chart_->legend()->hide();

        chart_->setBackgroundBrush(Qt::NoBrush);
        chart_->setBackgroundRoundness(0);
        chart_->setMargins(QMargins(8, 8, 8, 8));

        QPen linePen(Qt::red, 2);
        odomSeries_->setPen(linePen);

        QPen diffLinePen(Qt::green, 1); 
        diffOdomSeries_->setPen(diffLinePen);

        QColor scatterColor(3, 252, 132); 
        odomScatterSeries_->setMarkerSize(5);
        odomScatterSeries_->setColor(Qt::red);
        odomScatterSeries_->setBrush(QBrush(Qt::red)); // သို့မဟုတ် QColor(255, 215, 0)
        odomScatterSeries_->setPen(QPen(Qt::red, 0.5)); // အနားသတ်မျဉ်း (Optional)

        QColor diffScatterColor(255, 165, 0); 
        diffOdomScatterSeries_->setMarkerSize(4);
        diffOdomScatterSeries_->setColor(Qt::green); 
        diffOdomScatterSeries_->setBrush(QBrush(Qt::green)); 
        diffOdomScatterSeries_->setPen(QPen(Qt::green, 0.5));       

        QColor axisColor(189, 183, 183, 80);
        axisX_->setGridLineColor(axisColor);
        axisY_->setGridLineColor(axisColor);
        axisX_->setLabelsColor(axisColor);
        axisY_->setLabelsColor(axisColor);
        axisX_->setLinePen(QPen(axisColor));
        axisY_->setLinePen(QPen(axisColor));

        axisX_->setLabelsAngle(-90);

        QFont titleFont;
        QColor titleColor("#03fc84");
        titleFont.setFamily("SF Pro Text");
        titleFont.setPointSize(10);
        chart_->setTitle("Odom & Diff Odom Positions (X, Y)"); 
        chart_->setTitleBrush(QBrush(titleColor));
        chart_->setTitleFont(titleFont);

        ////////////////////////////

        QFont axisFont;
        axisFont.setFamily("SF Pro Text");
        axisFont.setPointSize(7);
        axisX_->setLabelsFont(axisFont);
        axisY_->setLabelsFont(axisFont);

        axisX_->setRange(-5, 5);
        axisX_->setTickCount(11);
        axisY_->setRange(-5, 5);
        axisY_->setTickCount(11);

        QVBoxLayout* layout = new QVBoxLayout(this);
        QChartView* chartView = new QChartView(chart_);
        chartView->setRenderHint(QPainter::Antialiasing);

        // NEW: Set QChartView size constraint for a square aspect ratio
        // This tells the layout to try and keep the chart square.
        chartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        chartView->setMinimumSize(300, 300); // Give a starting square size
        chartView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        layout->addWidget(chartView);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    
void updateGraph(const QPointF& my_odomPosition, const QPointF& my_diffOdomPosition)
{

    QPointF odomPosition(-1.0 * my_odomPosition.y(), my_odomPosition.x());
    QPointF diffOdomPosition(-1.0 * my_diffOdomPosition.y(), my_diffOdomPosition.x());

    // 1. Sliding Window Logic for BOTH Paths
    if (odomPositions_.size() >= MAX_POINTS) {
        odomPositions_.removeFirst();
        diffOdomPositions_.removeFirst(); // Keep both lists synchronized
    }
    odomPositions_.append(odomPosition);
    diffOdomPositions_.append(diffOdomPosition);

    // 2. Clear and Redraw ALL Series
    odomSeries_->clear();
    odomScatterSeries_->clear();
    diffOdomSeries_->clear();
    diffOdomScatterSeries_->clear();

    if (odomPositions_.isEmpty())
    {
        // Reset axes if no data
        axisX_->setRange(-1, 1);
        axisY_->setRange(-1, 1);
        axisX_->setTickCount(5);
        axisY_->setTickCount(5);
        return;
    }

    // 3. Auto-calculate Axes Range based on ALL points
    static double minX = std::numeric_limits<double>::max();
    static double maxX = std::numeric_limits<double>::lowest();
    static double minY = std::numeric_limits<double>::max();
    static double maxY = std::numeric_limits<double>::lowest();

    // Combine Odom and Diff Odom points for Auto-Scaling
    auto updateMinMax = [&](const QPointF& p) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    };

    // if(odomPosition.x() > maxX || odomPosition.x() < minX || odomPosition.y() > maxY || odomPosition.y() < minY)
    // {
    //     updateMinMax(odomPosition);
    // }
    double shift_offset = 0.05; 
    if(odomPosition.x() > maxX )
    {
        minX += shift_offset; maxX += shift_offset;
    }
    if(odomPosition.x() < minX )
    {
        minX -= shift_offset; maxX -= shift_offset;
    }
    if(odomPosition.y() > maxY )
    {
        minY += shift_offset; maxY += shift_offset;
    }
    if(odomPosition.y() < minY )
    {
        minY -= shift_offset; maxY -= shift_offset;
    }
    if(first_time_update_)
    {
        first_time_update_ = false;
        updateMinMax(odomPosition);
        //updateMinMax(diffOdomPosition);
    }

    // for (const auto& p : odomPositions_) {
    //     updateMinMax(p);
    // }
    // for (const auto& p : diffOdomPositions_) {
    //     updateMinMax(p);
    // }
    
    // Add Padding to the calculated range (e.g., 5% of the range or a fixed small value)
    const double padding = 0.1; // 0.1 meter padding for better visualization
    
    // Handle case where only one point exists (range is zero)
    double rangeX = maxX - minX;
    double rangeY = maxY - minY;
    
    if (rangeX < 0.1) rangeX = 0.2; // Ensure min range is not too small
    if (rangeY < 0.1) rangeY = 0.2;
    
    double padX = rangeX * 0.05 + padding; // 5% of range + 0.1m fixed pad
    double padY = rangeY * 0.05 + padding;
    
    // Apply the new ranges with padding
    axisX_->setRange(minX - padX, maxX + padX);
    axisY_->setRange(minY - padY, maxY + padY);

    // 4. Populate ALL Series (Loop)
    for (int i = 0; i < odomPositions_.size(); ++i) {
        // Odom Path
        odomSeries_->append(odomPositions_[i]);
        odomScatterSeries_->append(odomPositions_[i]);
        
        // Diff Odom Path
        diffOdomSeries_->append(diffOdomPositions_[i]);
        diffOdomScatterSeries_->append(diffOdomPositions_[i]);
    }

    // 5. Set Tick Count
    // Use an appropriate tick count based on the dynamic range (e.g., 10 ticks per meter)
    int numTicksX = qRound((maxX - minX) / 0.1); 
    int numTicksY = qRound((maxY - minY) / 0.1); 
    
    // Ensure minimum 5 ticks and maximum 20 ticks
    axisX_->setTickCount(qBound(5, numTicksX, 20)); 
    axisY_->setTickCount(qBound(5, numTicksY, 20));
}

    // Destructor
    virtual ~RomPositionGraph() {}

private:
    QChart *chart_ = nullptr;

    // Odom Path (Series 1)
    QLineSeries *odomSeries_ = nullptr; 
    QScatterSeries *odomScatterSeries_ = nullptr;
    QList<QPointF> odomPositions_; 
    
    // Diff Odom Path (Series 2)
    QLineSeries *diffOdomSeries_ = nullptr;
    QScatterSeries *diffOdomScatterSeries_ = nullptr;
    QList<QPointF> diffOdomPositions_; 
    
    QValueAxis *axisX_ = nullptr;
    QValueAxis *axisY_ = nullptr;

    bool first_time_update_ = true;
    
};


// Custom arrow class to represent a heading (UNMODIFIED)
class HeadingArrow : public QGraphicsLineItem
{
// ... (HeadingArrow definition remains the same) ...
public:
    HeadingArrow(qreal length, QGraphicsItem *parent = nullptr)
        : QGraphicsLineItem(parent)
    {
        setLine(0, 0, length, 0); 
        setTransformOriginPoint(0, 0);
        setPen(QPen(Qt::white, 2));

        QPolygonF head;
        head << QPointF(length, 0)      
             << QPointF(length - 8, -4) 
             << QPointF(length - 8, 4); 
        
        arrowHead_ = new QGraphicsPolygonItem(head, this);
        arrowHead_->setBrush(Qt::white);
        arrowHead_->setPen(Qt::NoPen);
    }

    void setColor(const QColor &color) {
        setPen(QPen(color, 2));
        arrowHead_->setBrush(color);
    }

    void setHeading(qreal angleDegrees, const QPointF& origin) {
        setRotation(angleDegrees);
        setPos(origin);
    }

private:
    QGraphicsPolygonItem *arrowHead_;
};


class RomPolarHeadingGraph : public QWidget {
    Q_OBJECT

public:
    RomPolarHeadingGraph(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        // FIX 1: QWidget Background Transparent
        // Note: For full transparency, the parent widget/container must also be configured.
        setStyleSheet("background-color: transparent;"); 

        chart_ = new QPolarChart();
        
        // FIX 2: Chart Background Transparent
        chart_->setBackgroundBrush(Qt::NoBrush);
        chart_->setPlotAreaBackgroundBrush(Qt::NoBrush); // Ensure plot area itself is transparent
        chart_->setPlotAreaBackgroundVisible(false);

        // Title and Title Color (Green as requested previously)
        chart_->setTitle("Odom, Diff Odom, IMU Headings");
        chart_->setTitleBrush(QBrush(QColor("#03fc84")));
        
        chart_->legend()->hide();

        // --- 1. Axis Setup ---

        QFont axisFont;
        axisFont.setFamily("SF Pro Text");
        axisFont.setPointSize(7); 
        
        // White color for all axis lines and labels
        QColor whiteColor(255, 255, 255); 
        QPen whitePen(whiteColor);
        
        QColor polarColor(189, 183, 183, 80);

        // Radial Axis (Radius)
        QValueAxis *radialAxis = new QValueAxis();
        radialAxis->setRange(0, 1);
        radialAxis->setLabelsVisible(false);
        radialAxis->setGridLineVisible(true); 
        radialAxis->setLinePen(Qt::NoPen);
        radialAxis->setGridLinePen(polarColor); 

        radialAxis->setLabelsFont(axisFont);


        QValueAxis *angularAxis = new QValueAxis();
        angularAxis->setRange(0, 360);
     
        angularAxis->setLabelsColor(polarColor);
        angularAxis->setGridLineColor(polarColor);
        angularAxis->setLinePen(polarColor);
        angularAxis->setTickCount(13);

        angularAxis->setLabelsFont(axisFont);

        chart_->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
        chart_->addAxis(radialAxis, QPolarChart::PolarOrientationRadial); 
        
        // --- 2. Dummy Series for Mapping (Unchanged) ---
        QScatterSeries *scatterSeries = new QScatterSeries();
        scatterSeries->append(0, 0);
        scatterSeries->setMarkerSize(0);
        chart_->addSeries(scatterSeries);

        dummyMapSeries_ = scatterSeries; 
        
        scatterSeries->attachAxis(radialAxis); 
        scatterSeries->attachAxis(angularAxis); 

        // --- 3. Arrow Initialization (Colors remain distinct but should be clearly visible) ---
        qreal arrowLength = 40;
        
        odomArrow_ = new HeadingArrow(arrowLength, chart_);
        odomArrow_->setColor(QColor(Qt::red));
        
        diffOdomArrow_ = new HeadingArrow(arrowLength, chart_);
        diffOdomArrow_->setColor(QColor(Qt::green)); 

        imuArrow_ = new HeadingArrow(arrowLength, chart_);
        imuArrow_->setColor(QColor(Qt::blue)); 
        
        odomArrow_->setVisible(false);
        diffOdomArrow_->setVisible(false);
        imuArrow_->setVisible(false);

        // --- 4. Layout ---
        QVBoxLayout* layout = new QVBoxLayout(this);
        QChartView* chartView = new QChartView(chart_);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRenderHint(QPainter::SmoothPixmapTransform);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        // Optional: Ensure QChartView background is also transparent
        chartView->setStyleSheet("background-color: transparent;"); 
        
        layout->addWidget(chartView);
        setLayout(layout);
    }

    // RomPolarHeadingGraph::updateGraph function အစားထိုးရန်

void updateGraph(double odomZ, double diffOdomZ, double imuZ)
{
    // ... (Radians to Degrees Conversion Utility remains the same) ...

    qreal odomDegrees = odomZ;
    qreal diffOdomDegrees = diffOdomZ;
    qreal imuDegrees = imuZ;

    // 1. Chart Plot Area ၏ ဗဟိုချက်ကို ရှာပါ။
    // QChart::plotArea() returns the rect in scene coordinates.
    const QRectF plotRect = chart_->plotArea();
    
    // Calculate the center point of the visual plot area
    QPointF chartCenter(
        plotRect.x() + plotRect.width() / 2,
        plotRect.y() + plotRect.height() / 2
    );
    
    // 2. Update Arrow Headings and Visibility (FIXED POSITIONING)
    
    odomArrow_->setHeading(odomDegrees, chartCenter);
    diffOdomArrow_->setHeading(diffOdomDegrees, chartCenter);
    imuArrow_->setHeading(imuDegrees, chartCenter);

    // ... (Z-Value setting remains the same) ...
    odomArrow_->setZValue(10);
    diffOdomArrow_->setZValue(10);
    imuArrow_->setZValue(10);

    odomArrow_->setVisible(true);
    diffOdomArrow_->setVisible(true);
    imuArrow_->setVisible(true);
}

    virtual ~RomPolarHeadingGraph() {}

private:
    QPolarChart *chart_ = nullptr;
    QAbstractSeries *dummyMapSeries_ = nullptr; 
    
    HeadingArrow *odomArrow_ = nullptr;
    HeadingArrow *diffOdomArrow_ = nullptr;
    HeadingArrow *imuArrow_ = nullptr;
};

}
#endif
