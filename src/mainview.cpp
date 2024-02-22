#include "mainview.h"

#include <QDateTime>
#include "vertex.h"

/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "MainView constructor";

  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
  qDebug() << "MainView destructor";

  makeCurrent();
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &EBO);
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL() {
  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  }

  QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  
  Vertex red(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
  Vertex blue(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
  Vertex yellow(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
  Vertex green(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
  Vertex purple(0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f);


  Vertex vertices[] = {
    red, green, yellow, blue, purple
  };
  
  GLuint indices[] = {
    0, 3, 2, 
    0, 2, 1, 
    0, 4, 3, 
    3, 4, 2, 
    2, 4, 1, 
    1, 4, 0
  };

  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);

  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  // Set the color to be used by glClear. This is, effectively, the background
  // color.
  glClearColor(0.37f, 0.42f, 0.45f, 0.0f);
  createShaderProgram();


  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (3 * sizeof(GLfloat)));
  
  // Set the value of modelView such that the pyramid is translated by (−2, 0, −6)
  modelView.setToIdentity();
  modelView.translate(QVector3D(-2.0f, 0.0f, -6.0f));

  // Set the value of projection 
  aspectRatio = 1.0f * width() / height();
  projection.setToIdentity();
  projection.perspective(60.0f, aspectRatio, 0.2f, 20.0f);


}

/**
 * @brief MainView::createShaderProgram Creates a new shader program with a
 * vertex and fragment shader.
 */
void MainView::createShaderProgram() {
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        ":/shaders/vertshader.glsl");
  shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        ":/shaders/fragshader.glsl");
  shaderProgram.link();
}

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL() {
  // Clear the screen before rendering
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glClearColor(0.37f, 0.42f, 0.45f, 0.0f);
  shaderProgram.bind();
  shaderProgram.setUniformValue("modelViewMatrix", modelView);
  shaderProgram.setUniformValue("projectionMatrix", projection);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
  // Draw here

  shaderProgram.release();
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  // TODO: Update projection to fit the new aspect ratio
  // Q_UNUSED(newWidth)
  // Q_UNUSED(newHeight)
  
  if (newHeight == 0){
    newHeight = 1;
  }
  aspectRatio = 1.0f * newWidth / newHeight;
  glViewport(0, 0, newWidth, newHeight);
  projection.setToIdentity();
  projection.perspective(60.0f, aspectRatio, 0.2f, 20.0f);

}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  // qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << ","
  //          << rotateZ << ")";
  // Q_UNIMPLEMENTED();
  rotate_X = rotateX;
  rotate_Y = rotateY;
  rotate_Z = rotateZ;
  modelView.setToIdentity();
  modelView.translate(QVector3D(-2.0f, 0.0f, -6.0f));
  modelView.rotate(rotate_X, QVector3D(1.0f, 0.0f, 0.0f));
  modelView.rotate(rotate_Y, QVector3D(0.0f, 1.0f, 0.0f));
  modelView.rotate(rotate_Z, QVector3D(0.0f, 0.0f, 1.0f));
  modelView.scale(ogscale);
  update();

}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float scale) {
  // qDebug() << "Scale changed to " << scale;

  // projection.setToIdentity();
  // projection.perspective(60.0f, aspectRatio, 0.2f, 20.0f);
  // projection.scale(scale);
  ogscale = scale;
  modelView.setToIdentity();
  modelView.translate(QVector3D(-2.0f, 0.0f, -6.0f));
  modelView.rotate(rotate_X, QVector3D(1.0f, 0.0f, 0.0f));
  modelView.rotate(rotate_Y, QVector3D(0.0f, 1.0f, 0.0f));
  modelView.rotate(rotate_Z, QVector3D(0.0f, 0.0f, 1.0f));
  modelView.scale(scale);

  update();
  // Q_UNIMPLEMENTED();
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
