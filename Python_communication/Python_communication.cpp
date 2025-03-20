#include <Python.h>
#include <iostream>
#include <vector>

//std::vector<std::tuple<std::string, double, double, double>> PyListToVector(PyObject* pList) {
//    std::vector<std::tuple<std::string, double, double, double>> vec;
//
//    if (PyList_Check(pList)) {
//        Py_ssize_t list_size = PyList_Size(pList);
//
//        for (Py_ssize_t i = 0; i < list_size; i++) {
//            PyObject* pItem = PyList_GetItem(pList, i); // Pobieramy element listy (tuple)
//
//            if (PyTuple_Check(pItem) && PyTuple_Size(pItem) == 4) {
//                vec.emplace_back(
//                    PyUnicode_AsUTF8(PyTuple_GetItem(pItem, 0)), // string
//                    PyFloat_AsDouble(PyTuple_GetItem(pItem, 1)), // double
//                    PyFloat_AsDouble(PyTuple_GetItem(pItem, 2)), // double
//                    PyFloat_AsDouble(PyTuple_GetItem(pItem, 3))  // double
//                );
//            }
//        }
//    }
//    return vec;
//}

void processDXF_Cpp(const std::string& file_path, const std::string& output_file) {
    // Inicjalizacja interpretera Pythona
    Py_Initialize();

    // Pobranie sys.path
    PyObject* sysPath = PySys_GetObject("path");

    // Dodanie katalogu, w którym znajduje się skrypt
    PyList_Append(sysPath, PyUnicode_DecodeFSDefault("C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer"));

    // Importowanie modułu 
    PyObject* pModule = PyImport_ImportModule("ReadDXF_system_exe");
    if (!pModule) {
        std::cerr << "Nie można załadować modułu ReadDXF_system_exe.py!" << std::endl;
        Py_Finalize();
        return;
    }

    // Pobranie funkcji process_dxf()
    PyObject* pFunc = PyObject_GetAttrString(pModule, "process_dxf");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        std::cerr << "Nie znaleziono funkcji process_dxf()" << std::endl;
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
        Py_Finalize();
        return;
    }

    // Konwersja argumentów do Pythona
    PyObject* pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(file_path.c_str()));
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(output_file.c_str()));

    // Wywołanie funkcji
    PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
    if (pValue == nullptr) {
        std::cerr << "Błąd podczas wywoływania process_dxf()" << std::endl;
        PyErr_Print();
    }
    else {
        std::cout << "Plik DXF przetworzony pomyślnie!" << std::endl;
    }

    //std::vector<std::tuple<std::string, double, double, double>> extracted_data = PyListToVector(pValue);

    //// Wyświetlanie wyników
    //for (const auto& item : extracted_data) {
    //    std::cout << std::get<0>(item) << " (" << std::get<1>(item) << ", "
    //        << std::get<2>(item) << ") - " << std::get<3>(item) << std::endl;
    //}

    // Sprzątanie pamięci
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    // Zamknięcie interpretera
    Py_Finalize();
}

int main() {
    std::string file_path = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer\\HDES_2_S083239103M_wieniec dolny.DXF";
    std::string output_file = "C:\\Home\\PROJEKTY\\2024-11-07 Ikea - pomiar plyt\\05.DXF newer\\output.csv";

    processDXF_Cpp(file_path, output_file);

    return 0;
}