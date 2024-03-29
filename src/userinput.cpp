#include <QDebug>

#include "mainview.h"

/**
 * @brief MainView::keyPressEvent Triggered by a key press.
 * @param ev Key event.
 */
void MainView::keyPressEvent(QKeyEvent *ev) {
  switch (ev->key()) {
    case 'A':
      qDebug() << "A pressed";
      break;
    default:
      // ev->key() is an integer. For alpha numeric characters keys it
      // equivalent with the char value ('A' == 65, '1' == 49) Alternatively,
      // you could use Qt Key enums, see http://doc.qt.io/qt-6/qt.html#Key-enum
      qDebug() << ev->key() << "pressed";
      break;
  }
  // Used to update the screen after changes
  update();
}

// Triggered by releasing a key
/**
 * @brief MainView::keyReleaseEvent Triggered by a key released.
 * @param ev Key event.
 */
void MainView::keyReleaseEvent(QKeyEvent *ev) {
  switch (ev->key()) {
    case 'A':
      // qDebug() << "A released";
      break;
    case 'R':
      setRotation(0, 0, 0);
      setScale(1.0f);
      break;
    default:
      // qDebug() << ev->key() << "released";
      break;
  }

  update();
}

/**
 * @brief MainView::mouseDoubleClickEvent Triggered by clicking two subsequent
 * times on any mouse button. It also fires two mousePress and mouseRelease
 * events.
 * @param ev Mouse events.
 */
void MainView::mouseDoubleClickEvent(QMouseEvent *ev) {
  // qDebug() << "Mouse double clicked:" << ev->button();
  if (ev->button() == Qt::LeftButton) {
    setScale(1.0f);
  } else if (ev->button() == Qt::RightButton) {
    setRotation(0, 0, 0);
  }
  
  update();
}

/**
 * @brief MainView::mouseMoveEvent Triggered when moving the mouse inside the
 window (only when the mouse is clicked).
 * @param ev Mouse event.
 */
void MainView::mouseMoveEvent(QMouseEvent *ev) {
  // qDebug() << "x" << ev->position().x() << "y" << ev->position().y();
  setRotation(ev->position().x(), ev->position().y(), 0);
  update();
}

/**
 * @brief MainView::mousePressEvent Triggered when pressing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mousePressEvent(QMouseEvent *ev) {
  qDebug() << "Mouse button pressed:" << ev->button();

  update();
  // Do not remove the line below, clicking must focus on this widget!
  this->setFocus();
}

/**
 * @brief MainView::mouseReleaseEvent Triggered when releasing any mouse button.
 * @param ev Mouse event.
 */
void MainView::mouseReleaseEvent(QMouseEvent *ev) {
  qDebug() << "Mouse button released" << ev->button();

  update();
}

/**
 * @brief MainView::wheelEvent Triggered when clicking scrolling with the scroll
 * wheel on the mouse.
 * @param ev Mouse event.
 */
void MainView::wheelEvent(QWheelEvent *ev) {
  // qDebug() << "Mouse wheel:" << ev->angleDelta();

  int delta = ev->angleDelta().y() > 0 ? 1 : -1;

  float minScale = 0.01f;
  float maxScale = 2.0f;

  float stepSize = 0.08f;

  float newScale = qBound(minScale, ogscale + delta * stepSize, maxScale);

  setScale(newScale);
  update();
}
