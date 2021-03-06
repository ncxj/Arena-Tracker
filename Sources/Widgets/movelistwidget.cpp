#include "movelistwidget.h"
#include "../Cards/deckcard.h"
#include "../themehandler.h"
#include <QtWidgets>

MoveListWidget::MoveListWidget(QWidget *parent) : QListWidget(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    this->setIconSize(10*CARD_SIZE);
    this->setTheme();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setFocusPolicy(Qt::NoFocus);
}


void MoveListWidget::setTheme()
{
    this->setStyleSheet(
        "QListView{background-color: transparent; outline: 0;}"
        "QListView::item{padding: 0px;}"
        "QListView::item:selected {background: " + ThemeHandler::bgSelectedItemListColor() + "; "
            "color: " + ThemeHandler::fgSelectedItemListColor() + ";}"
    );
}


void MoveListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListWidget::mouseDoubleClickEvent(event);
    event->ignore();
}
void MoveListWidget::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent(event);
    event->ignore();
}
void MoveListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);
    event->ignore();
}
void MoveListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QListWidget::mouseReleaseEvent(event);
    event->ignore();
}


void MoveListWidget::leaveEvent(QEvent * e)
{
    QListWidget::leaveEvent(e);

    int mouseX = this->mapFromGlobal(QCursor::pos()).x();
    if(mouseX < 0 || mouseX >= size().width())  emit xLeave();
    emit leave();
}
