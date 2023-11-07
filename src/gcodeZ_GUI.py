import sys
import os
import platform
import subprocess
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel, QFileDialog, QDoubleSpinBox, QTextEdit, QCheckBox
from PyQt5.QtCore import QProcess, QTextCodec, QByteArray, Qt


class GCodeConverterApp(QWidget):
    def __init__(self):
        super().__init__()
        self.current_os = platform.system()
        self.stl_file = ""
        self.gcode_file = ""
        self.output_file = ""
        self.process = QProcess(self)
        self.initUI()

    def initUI(self):
        self.setWindowTitle("GCodeZ - G-Code Converter")
        self.setGeometry(100, 100, 400, 500)

        self.stl_label = QLabel("Choose STL file:")
        self.stl_button = QPushButton("Browse")
        self.stl_button.clicked.connect(self.chooseSTLFile)

        self.gcode_label = QLabel("Choose G-code file:")
        self.gcode_button = QPushButton("Browse")
        self.gcode_button.clicked.connect(self.chooseGCodeFile)

        self.output_label = QLabel("Choose output file:")
        self.output_button = QPushButton("Browse")
        self.output_button.clicked.connect(self.chooseOutputFile)

        self.laser_label = QLabel("Set laser diameter (0.05 - 0.7):")
        self.laser_spinbox = QDoubleSpinBox()
        self.laser_spinbox.setRange(0.05, 0.7)
        self.laser_spinbox.setSingleStep(0.01)
        self.laser_spinbox.setValue(0.05)
		
        self.stepwidth_label = QLabel("Set stepwith (0.1 - 5.0):")
        self.stepwidth_spinbox = QDoubleSpinBox()
        self.stepwidth_spinbox.setRange(0.1, 5.0)
        self.stepwidth_spinbox.setSingleStep(0.1)
        self.stepwidth_spinbox.setValue(0.5)
		
        self.simplify_label = QLabel("Simplify STL:")
        self.simplify_checkbox = QCheckBox("Simplifiy STL")
        self.simplify_checkbox.setCheckState(Qt.Unchecked)
		
        self.execute_button = QPushButton("Execute")
        self.execute_button.clicked.connect(self.executeCommand)

        self.log_text = QTextEdit()

        vbox = QVBoxLayout()
        vbox.addWidget(self.stl_label)
        vbox.addWidget(self.stl_button)
        vbox.addWidget(self.gcode_label)
        vbox.addWidget(self.gcode_button)
        vbox.addWidget(self.output_label)
        vbox.addWidget(self.output_button)
        vbox.addWidget(self.laser_label)
        vbox.addWidget(self.laser_spinbox)
        vbox.addWidget(self.stepwidth_label)
        vbox.addWidget(self.stepwidth_spinbox)
        #vbox.addWidget(self.simplify_label)
        vbox.addWidget(self.simplify_checkbox)
        vbox.addWidget(self.execute_button)
        vbox.addWidget(self.log_text)

        self.setLayout(vbox)

    def chooseSTLFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly
        fileName, _ = QFileDialog.getOpenFileName(self, "Choose STL File", "", "STL Files (*.stl);;All Files (*)", options=options)
        if fileName:
            self.stl_button.setText(os.path.basename(fileName))
            self.stl_file = fileName

    def chooseGCodeFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly
        fileName, _ = QFileDialog.getOpenFileName(self, "Choose G-code File", "", "G-code Files (*.nc);;All Files (*)", options=options)
        if fileName:
            self.gcode_button.setText(os.path.basename(fileName))
            self.gcode_file = fileName

    def chooseOutputFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly
        fileName, _ = QFileDialog.getSaveFileName(self, "Choose Output File", "", "G-code Files (*.nc);;All Files (*)", options=options)
        if fileName:
            self.output_button.setText(os.path.basename(fileName))
            self.output_file = fileName

    def executeCommand(self):

        self.log_text.clear()
	
        stl_file = self.stl_file
        gcode_file = self.gcode_file
        output_file = self.output_file
        laser_diameter = self.laser_spinbox.value()
        stepwidth = self.stepwidth_spinbox.value()

        if self.current_os == "Windows":
            executable_name = "gcodeZ.exe"
        else:
            executable_name = "gcodeZ"

        command = f"{executable_name} --stl=\"{stl_file}\" --gcode=\"{gcode_file}\" --output=\"{output_file}\" --laser={laser_diameter} --stepwidth={stepwidth}"
        print(command)
		
        if(self.simplify_checkbox.isChecked()):
            command += " --simplify"

        try:
            # Set the working directory for the process
            self.process.setWorkingDirectory(os.path.dirname(__file__))

            # Start the command
            self.process.start(command)

            # Connect the process signals to capture the output
            self.process.readyReadStandardOutput.connect(self.onReadyReadStandardOutput)
            self.process.readyReadStandardError.connect(self.onReadyReadStandardError)
            self.process.finished.connect(self.onProcessFinished)
        except Exception as e:
            self.log_text.clear()
            self.log_text.append(f"Error: {str(e)}")

    def onReadyReadStandardOutput(self):
        data = self.process.readAllStandardOutput()
        text = QTextCodec.codecForUtfText(data).toUnicode(data)
        self.log_text.append(text.strip())

    def onReadyReadStandardError(self):
        data = self.process.readAllStandardError()
        text = QTextCodec.codecForUtfText(data).toUnicode(data)
        self.log_text.append("Command Error:")
        self.log_text.append(text.strip())

    def onProcessFinished(self, exitCode, exitStatus):
        if exitCode != 0:
            self.log_text.append(f"Command exited with code {exitCode}.")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = GCodeConverterApp()
    window.show()
    sys.exit(app.exec_())
