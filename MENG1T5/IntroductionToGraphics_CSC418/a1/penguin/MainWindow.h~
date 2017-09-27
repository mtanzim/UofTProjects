/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1
  
  
    Instructions:
        See main.cpp for more instructions.

        This file contains the class for the main window of the program.
**************************************************************************/

#ifndef __MainWindow_h__
#define __MainWindow_h__

#include <QtGui/QtGui>
#include "GLWidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow()
    {
        // Create a GLWidget to hold the OpenGL viewport.
        m_gl_widget = new GLWidget();

        // Create a checkbox to turn animation on and off, and make it
        // call GLWidget::onPressAnimate when checked.
        m_animate_checkbox = new QCheckBox("Animate", this);
        connect(
            m_animate_checkbox, SIGNAL(stateChanged(int)),
            m_gl_widget, SLOT(onPressAnimate(int)));

        // Create a button to quit the program.
        m_quit_button = new QPushButton("Quit", this);
        connect(
            m_quit_button, SIGNAL(clicked(bool)),
            this, SLOT(onPressQuit(bool)));

        m_main_layout = new QVBoxLayout();
        m_main_layout->addWidget(m_gl_widget);

        // Create a slider to control the joint angle, and make it call
        // GLWidget::setJointAngle when the slider value changes.


        //////////////////////////////////////////////////////
        // TODO: Add additional joint sliders here
        //////////////////////////////////////////////////////

	//added additional joint slider to individual movement
        m_slider = create_joint_angle_slider(
	    "Horzontal", GLWidget::X_JOINT_MIN, GLWidget::X_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setXAngle(int)));

        m_slider = create_joint_angle_slider(
	    "Vertical", GLWidget::Y_JOINT_MIN, GLWidget::Y_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setYAngle(int)));

        m_slider = create_joint_angle_slider(
	    "Arm", GLWidget::ARM_JOINT_MIN, GLWidget::ARM_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setJointAngle(int)));
			
        m_slider = create_joint_angle_slider(
	    "Head", GLWidget::HEAD_JOINT_MIN, GLWidget::HEAD_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setHeadJointAngle(int)));


        m_slider = create_joint_angle_slider(
	    "Beak", GLWidget::BEAK_JOINT_MIN, GLWidget::BEAK_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setBeakJointAngle(int)));


        m_slider = create_joint_angle_slider(
	    "Left Toe", GLWidget::LL_JOINT_MIN, GLWidget::LL_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setllJointAngle(int)));


        m_slider = create_joint_angle_slider(
	    "Left Leg", GLWidget::LU_JOINT_MIN, GLWidget::LU_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setluJointAngle(int)));



        m_slider = create_joint_angle_slider(
	    "Right Toe", GLWidget::RL_JOINT_MIN, GLWidget::RL_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setrlJointAngle(int)));


        m_slider = create_joint_angle_slider(
	    "Right Leg", GLWidget::RU_JOINT_MIN, GLWidget::RU_JOINT_MAX);
        connect(
            m_slider, SIGNAL(valueChanged(int)),
            m_gl_widget, SLOT(setruJointAngle(int)));

        m_main_layout->addWidget(m_animate_checkbox);
        m_main_layout->addWidget(m_quit_button);
        setLayout(m_main_layout);

        m_slider->setValue(0);
        setWindowTitle("CSC418/2504 Assignment 1 by Tanzim Mokammel");
    }

public slots:
    void onPressQuit(bool)
    {
        exit(0);
    }

private:
    QSlider *create_joint_angle_slider(
	const char *label, int min_angle, int max_angle)
    {
        QSlider *slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(min_angle, max_angle);
        slider->setSingleStep(1);
        slider->setPageStep(5);
        slider->setTickInterval(5);
        slider->setTickPosition(QSlider::TicksBelow);

	QBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(new QLabel(label));
	layout->addWidget(slider);
	m_main_layout->addLayout(layout);

        return slider;
    }

    GLWidget *m_gl_widget;
    QCheckBox *m_animate_checkbox;
    QPushButton *m_quit_button;
    QSlider *m_slider;
    QVBoxLayout *m_main_layout;
};

#endif
