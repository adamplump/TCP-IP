#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dxflib/dl_dxf.h>
#include <dxflib/dl_creationadapter.h>

class DXFProcessor : public DL_CreationAdapter {
public:
    std::vector<std::vector<std::string>> data; // Przechowywanie odczytanych obiektów DXF

    // Obs³uga linii
    void addLine(const DL_LineData& line) override {
        data.push_back({
            "LINE",
            std::to_string(line.x1), std::to_string(line.y1),
            std::to_string(line.x2), std::to_string(line.y2)
            });

        std::cout << "LINE: (" << line.x1 << ", " << line.y1 << ") -> (" << line.x2 << ", " << line.y2 << ")\n";
    }

    // Obs³uga okrêgów
    void addCircle(const DL_CircleData& circle) override {
        data.push_back({
            "CIRCLE",
            std::to_string(circle.cx), std::to_string(circle.cy),
            std::to_string(circle.radius)
            });

        std::cout << "CIRCLE: (" << circle.cx << ", " << circle.cy << "), R=" << circle.radius << "\n";
    }

    // Obs³uga ³uków
    void addArc(const DL_ArcData& arc) override {
        data.push_back({
            "ARC",
            std::to_string(arc.cx), std::to_string(arc.cy),
            std::to_string(arc.radius)
            });

        std::cout << "ARC: (" << arc.cx << ", " << arc.cy << "), R=" << arc.radius << "\n";
    }

    // Obs³uga bloków INSERT
    void addInsert(const DL_InsertData& insert) override {
        data.push_back({
            "INSERT",
            std::to_string(insert.ipx), std::to_string(insert.ipy)
            });

        std::cout << "INSERT: (" << insert.ipx << ", " << insert.ipy << ")\n";
    }

    // Zapis do pliku CSV
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Nie uda³o siê otworzyæ pliku do zapisu!\n";
            return;
        }

        file << "Type,X,Y,Radius/Length\n";
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
        std::cout << "Dane zapisane do: " << filename << "\n";
    }
};

// Funkcja do przetwarzania pliku DXF
void processDXF(const std::string& filePath, const std::string& outputFile) {
    DXFProcessor processor;
    DL_Dxf dxf;

    if (!dxf.in(filePath, &processor)) {
        std::cerr << "B³¹d podczas wczytywania pliku DXF: " << filePath << "\n";
        return;
    }

    processor.saveToFile(outputFile);
}

int main() {
    std::string dxfFile = "test.dxf";   // Plik DXF do przetworzenia
    std::string outputCSV = "output.csv"; // Plik wynikowy CSV

    processDXF(dxfFile, outputCSV);
    return 0;
}