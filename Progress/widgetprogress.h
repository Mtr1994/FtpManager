#ifndef WIDGETPROGRESS_H
#define WIDGETPROGRESS_H

#include <QWidget>
#include <QPaintEvent>

class WidgetProgress : public QWidget
{
    Q_OBJECT
public:
    enum {P_Liner = 1, P_Cicle};
    explicit WidgetProgress(QWidget *parent = nullptr);

    float getValue() const;
    void setValue(float newValue);

    const QColor &getColor() const;
    void setColor(const QColor &newColor);

    bool getShowLabel() const;
    void setShowLabel(bool newShowLable);

    const QColor &getLabelColor() const;
    void setLabelColor(const QColor &newLabelColor);

    int getProgressType() const;
    void setProgressType(int newProgressType);

    int getCicleWidth() const;
    void setCicleWidth(int newCicleWidth);

signals:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void init();

    void drawLiner();
    void drawCicle();

private:
    QColor mBackgroundColor = { 235,238, 245, 255 };
    QColor mColor = { 24,144, 255, 255 };
    QColor mLabelColor = { 96, 98, 102, 255 };

    uint16_t mProgressHeight = 0;

    uint64_t mValue = 0;
    uint64_t mMinValue = 0;
    uint64_t mMaxValue = 100;

    // 是否显示进度文本
    bool mShowLabel = true;

    // 进度条类型
    int mProgressType = P_Liner;

    // 环形进度条宽度
    int mCicleWidth = 0;
};

#endif // WIDGETPROGRESS_H
