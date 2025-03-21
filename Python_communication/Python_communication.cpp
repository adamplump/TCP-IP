#include <Python.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <chrono>


struct Element {
    std::string type;
    std::vector<float> values;
};


std::vector<Element> PyListToVector(PyObject* pList) {
    std::vector<Element> result;
    std::vector<float> values;

    if (PyList_Check(pList)) {
        Py_ssize_t list_size = PyList_Size(pList);

        for (Py_ssize_t i = 0; i < list_size; i++) {
            PyObject* pItem = PyList_GetItem(pList, i); // Pobieramy element listy (tuple)

            /* Wyświetlanie w konsoli dla debugu */
            //PyObject* repr = PyObject_Repr(pItem);  // Zwraca string reprezentację obiektu jak w Pythonie
            //const char* str = PyUnicode_AsUTF8(repr);
            //std::cout << "pItem[" << i << "]: " << str << std::endl;
            //Py_DECREF(repr);  // Sprzątamy po sobie

            bool list_check = PyList_Check(pItem); // sprawdzenie czy jest to lista 
            int list_size = PyList_Size(pItem); 

            if (PyList_Check(pItem)) {
                PyObject* pyStr = PyList_GetItem(pItem, 0);
                Element elem;
                elem.type = PyUnicode_AsUTF8(pyStr);
          
                // pozostałe elementy to floaty
                for (Py_ssize_t j = 1; j < PyList_Size(pItem); ++j) {
                    PyObject* pyVal = PyList_GetItem(pItem, j);
                    if (PyFloat_Check(pyVal) || PyLong_Check(pyVal)) {
                        elem.values.push_back(static_cast<float>(PyFloat_AsDouble(pyVal)));
                    }
                }

                result.push_back(elem);
            }
        }
    }
    return result;
}

std::vector<Element> processDXF_Cpp(const std::string& file_path, const std::string& output_file) {

    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
    std::chrono::duration<double, std::milli> elapsedTimer;

    start = std::chrono::high_resolution_clock::now(); 
    // Inicjalizacja interpretera Pythona
    Py_Initialize();
    std::vector<Element> extracted_data;

    // Pobranie sys.path
    PyObject* sysPath = PySys_GetObject("path");
    // Dodanie katalogu, w którym znajduje się skrypt
    PyList_Append(sysPath, PyUnicode_DecodeFSDefault("C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer"));

    end = std::chrono::high_resolution_clock::now();
    elapsedTimer = end - start;
    std::cout << "czas inicjalizacji interpretera: " << elapsedTimer.count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Importowanie modułu 
    PyObject* pModule = PyImport_ImportModule("ReadDXF_system_exe");
    if (!pModule) {
        std::cerr << "Nie można załadować modułu ReadDXF_system_exe.py!" << std::endl;
        Py_Finalize();
        return extracted_data;
    }
    // Benchmark
    end = std::chrono::high_resolution_clock::now();
    elapsedTimer = end - start;
    std::cout << "czas importowania modułu: " << elapsedTimer.count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Pobranie funkcji process_dxf()
    PyObject* pFunc = PyObject_GetAttrString(pModule, "process_dxf");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        std::cerr << "Nie znaleziono funkcji process_dxf()" << std::endl;
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
        Py_Finalize();
        return extracted_data;
    }

    // Konwersja argumentów do Pythona
    PyObject* pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(file_path.c_str()));
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(output_file.c_str()));
    // Benchmark
    end = std::chrono::high_resolution_clock::now();
    elapsedTimer = end - start;
    std::cout << "czas konwersji argumentow: " << elapsedTimer.count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Wywołanie funkcji
    PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
    if (pValue == nullptr) {
        std::cerr << "Błąd podczas wywoływania process_dxf()" << std::endl;
        PyErr_Print();
    }
    else {
        std::cout << "Plik DXF przetworzony pomyślnie!" << std::endl;
    }
    extracted_data = PyListToVector(pValue);
    // Benchmark
    end = std::chrono::high_resolution_clock::now();
    elapsedTimer = end - start;
    std::cout << "czas konwersji argumentow: " << elapsedTimer.count() << std::endl;

    // Sprzątanie pamięci
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    // Zamknięcie interpretera
    Py_Finalize();

    return extracted_data;
}

int main() {
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
    std::chrono::duration<double, std::milli> elapsedTimer;
    std::vector<Element>extracted_data;

    start = std::chrono::high_resolution_clock::now(); // Start clock
    std::string file_path = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer\\HDES_2_S083239103M_wieniec dolny.DXF";
    std::string output_file = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer\\output.csv";

    extracted_data = processDXF_Cpp(file_path, output_file);

    // Benchmark
    end = std::chrono::high_resolution_clock::now();
    elapsedTimer = end - start;
    std::cout << "czas odczytu dxf: " << elapsedTimer.count() << std::endl;

    // Wyświetlanie wyników
    for (const auto& elem : extracted_data) {
        std::cout << elem.type << ": ";
        for (float v : elem.values) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}