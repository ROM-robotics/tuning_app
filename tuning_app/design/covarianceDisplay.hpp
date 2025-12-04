#ifndef ROM_COVARIANCE_GRAPHS_HPP
#define ROM_COVARIANCE_GRAPHS_HPP

#pragma once

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QPolarChart> // For Yaw visualization
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <Eigen/Dense> // For Eigen decomposition

namespace rom_dynamics::ui::qt {
class RomPositionCovarianceGraph : public QWidget
{
    Q_OBJECT

public:
    explicit RomPositionCovarianceGraph(QWidget *parent = nullptr);
    ~RomPositionCovarianceGraph() override = default;

    void updateGraph(double ekf_pose_x, 
                     double ekf_pose_y, 
                     const Eigen::Matrix2d& covariance_xy);

private:
    QChartView *chart_view_;
    QChart *chart_;
    QScatterSeries *ellipse_series_;
    QValueAxis *axisX_;
    QValueAxis *axisY_;
    
    double current_x_ = 0.0;
    double current_y_ = 0.0;
    
    void initializeChart();
    void drawPositionEllipse(const Eigen::Matrix2d& covariance_xy);
};

class RomYawCovarianceGraph : public QWidget
{
    Q_OBJECT

public:
    explicit RomYawCovarianceGraph(QWidget *parent = nullptr);
    ~RomYawCovarianceGraph() override = default;

    void updateGraph(double ekf_pose_yaw, double yaw_variance);

private:
    QChartView *chart_view_;
    QPolarChart *chart_;
    QLineSeries *yaw_arc_series_;
    
    double current_yaw_ = 0.0;
    
    void initializeChart();
    void drawYawSector(double yaw, double yaw_variance);
    
    qreal radiansToDegrees(double radians) const {
        qreal degrees = qRadiansToDegrees(radians);
        return fmod(degrees + 360.0, 360.0); 
    }
};

}

#endif // ROM_COVARIANCE_GRAPHS_HPP
