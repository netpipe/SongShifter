#include <QApplication>
#include <QFileDialog>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QProcess>
#include <QDebug>
#include <QMediaPlayer>

class PitchShifterApp : public QWidget {
    Q_OBJECT

public:
    PitchShifterApp(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout;

        // File selection button
        QPushButton *selectFileButton = new QPushButton("Select Audio File", this);
        layout->addWidget(selectFileButton);

        // Slider for pitch shift
        QLabel *pitchLabel = new QLabel("Pitch Shift (Semitones): 0", this);
        QSlider *pitchSlider = new QSlider(Qt::Horizontal, this);
        pitchSlider->setRange(-12, 12); // Range from -12 to +12 semitones
        layout->addWidget(pitchLabel);
        layout->addWidget(pitchSlider);

        // Connect the slider to pitch change
        connect(pitchSlider, &QSlider::valueChanged, [=](int value) {
            pitchLabel->setText(QString("Pitch Shift (Semitones): %1").arg(value));
            pitchShiftSemitones = value;
            double pitchFactor = (value + 12) / 24.0 * 2.0;//std::pow(2.0, value / 12.0);
            pitchFactor = std::max(0.16, std::min(2.0, pitchFactor));
               qDebug() << pitchFactor;
        });

        // Play button
        QPushButton *playButton = new QPushButton("Shift and Save Audio", this);
        layout->addWidget(playButton);

        // Set up the main layout
        setLayout(layout);

        // Connect file selection and play buttons
        connect(selectFileButton, &QPushButton::clicked, this, &PitchShifterApp::selectFile);
        connect(playButton, &QPushButton::clicked, this, &PitchShifterApp::shiftAndSaveAudio);
    }

private slots:
    void selectFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Audio File", "", "Audio Files (*.mp3 *.ogg)");
        if (!fileName.isEmpty()) {
            inputFilePath = fileName;
            qDebug() << "Selected file:" << inputFilePath;
        }
    }
    void shiftAndSaveAudio() {
        if (inputFilePath.isEmpty()) {
            qWarning() << "No file selected!";
            return;
        }
std::string test = inputFilePath.toStdString();
//std::string test2 =
        //QString outputFilePath = QFileDialog::getSaveFileName(this, "Save Shifted Audio", "/tmp/outputfile.mp3", "Audio Files (*.mp3)");
size_t lastDot = test.find_last_of('.');
QString outputFilePath;
if (lastDot != std::string::npos) {
    // Create a new output file path with the same base name and "_shifted.ogg" extension
    std::string outputFileName = test.substr(0, lastDot) + "_shifted.ogg";

    // Convert back to QString for Qt compatibility
     outputFilePath = QString::fromStdString(outputFileName);
} //else {
    // Handle the case where there is no extension (optional)
    // outputFilePath = QString::fromStdString(test + "_shifted.ogg");
//}
        if (outputFilePath.isEmpty()) {
            qWarning() << "No save location selected!";
            return;
        }

        double pitchFactor = (pitchShiftSemitones + 12) / 24.0 * 2.0;// std::pow(2.0, pitchShiftSemitones / 12);
        pitchFactor = std::max(0.16, std::min(2.0, pitchFactor));
        QString pitchShiftCmd = QString("asetrate=44100*%1,atempo=1/%1").arg(pitchFactor);

        std::max(0.0, std::min(2.0, pitchFactor));
        qDebug() << pitchFactor;

        QProcess ffmpeg;
#ifdef __APPLE__
        QString command = QString("/Applications/SongShifter.app/Contents/MacOS/ffmpeg -i \"%1\" -af \"%2\" \"%3\"")
            .arg(inputFilePath, pitchShiftCmd, outputFilePath);
#else
        QString command = QString("ffmpeg -i \"%1\" -af \"%2\" \"%3\"")
            .arg(inputFilePath, pitchShiftCmd, outputFilePath);
#endif
        qDebug() << "Running command:" << command;

        ffmpeg.start("/bin/bash", QStringList() << "-c" << command);
        bool finished = ffmpeg.waitForFinished();

        if (!finished) {
            qWarning() << "Pitch shift process failed to complete.";
        } else {
            qDebug() << "FFmpeg output:" << ffmpeg.readAllStandardOutput();
            qDebug() << "FFmpeg errors:" << ffmpeg.readAllStandardError();
        }

        QFile outputFile(outputFilePath);
        if (!outputFile.exists() || outputFile.size() == 0) {
            qWarning() << "Pitch shift failed!";
            return;
        }

        qDebug() << "Pitch shift successful. Saved to:" << outputFilePath;

      //  QMediaPlayer *player = new QMediaPlayer(this);
      //  player->setMedia(QUrl::fromLocalFile(outputFilePath));
       // player->play();
    }


private:
    QString inputFilePath;
    int pitchShiftSemitones = 0; // Stores the pitch shift value in semitones
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PitchShifterApp window;
    window.setWindowTitle("FFmpeg Pitch Shifter");
    window.show();

    return app.exec();
}

#include "main.moc"
