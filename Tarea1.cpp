#include <iostream>
#include <vector>
#include <string>
#include <memory>
// necesario para utilizar puntero unique_ptr

using namespace std;

// Clase primaria(abstracta), es la base estandar de la cual se va a trabajar. Se crea como una pregunta de la cual sirve de base para todas las preguntas
class Pregunta {
protected:
    int id;
    string enunciado;
    string nivelBloom;
    int tiempoEstimado;
    string solucionEsperada;
    int anio;

public: // Constructor de la clase que recibe lo necesario
    Pregunta(int id_, string enun, string nivel, int tiempo, string solucion, int anio_)
        : id(id_), enunciado(enun), nivelBloom(nivel), tiempoEstimado(tiempo), solucionEsperada(solucion), anio(anio_) {}

    // Metodos virtuales que se deben implementar en las subclases que se utilizaran
    virtual void mostrar() const = 0;
    virtual ~Pregunta() = default; // Destructor

    // Metodos de accesos para los atributos
    int getId() const { return id; }
    string getNivel() const { return nivelBloom; }
    int getTiempo() const { return tiempoEstimado; }
    int getAnio() const { return anio; }
};

// Clase de pregunta con seleccion multiple (Subclase)
class PreguntaOpcionMultiple : public Pregunta {
private:
    vector<string> opciones; // Lista de opciones
    int respuestaCorrecta;

public: // Constructor de la clase que recibe lo necesario
    PreguntaOpcionMultiple(int id_, string enun, string nivel, int tiempo, string solucion, int anio_,
                           vector<string> opc, int respuesta)
        : Pregunta(id_, enun, nivel, tiempo, solucion, anio_), opciones(opc), respuestaCorrecta(respuesta) {}

    void mostrar() const override {
        cout << "ID: " << id << " (Opcion Multiple)\nEnunciado: " << enunciado << "\nNivel: " << nivelBloom
             << "\nOpciones:\n";
        for (size_t i = 0; i < opciones.size(); i++) {
            cout << "  " << i + 1 << ") " << opciones[i] << endl;
        }
        cout << "Solucion: " << solucionEsperada << "\nAnio: " << anio << "\n\n";
    }
};

// Clase de pregunta con verdadero o falso. (Subclase)
class PreguntaVerdaderoFalso : public Pregunta {
private:
    bool respuesta; // Aqui booleano que trabaja con True o False en la respuesta

public:
    PreguntaVerdaderoFalso(int id_, string enun, string nivel, int tiempo, string solucion, int anio_, bool resp)
        : Pregunta(id_, enun, nivel, tiempo, solucion, anio_), respuesta(resp) {}

    void mostrar() const override {
        cout << "ID: " << id << " (Verdadero/Falso)\nEnunciado: " << enunciado << "\nNivel: " << nivelBloom
             << "\nRespuesta: " << (respuesta ? "Verdadero" : "Falso") << "\nAnio: " << anio << "\n\n";
    }
};

// Clase de la evaluacion que se representa construida por las preguntas
class Evaluacion {
private:
    vector<Pregunta*> preguntas; // Puntero para las preguntas
    string asignatura;

public:
    Evaluacion(string asig) : asignatura(asig) {}

    // Metodo para agregar una pregunta en la evaluacion
    void agregarPregunta(Pregunta* p) {
        preguntas.push_back(p);
    }

    // Se suman todos los tiempos estimados de las preguntas
    int calcularTiempoTotal() const {
        int total = 0;
        for (auto p : preguntas)
            total += p->getTiempo();
        return total;
    }

    // Metodo para mostrar la evaluacion al final completa
    void mostrarEvaluacion() const {
        cout << "\nEvaluacion de " << asignatura << "\n";
        for (auto p : preguntas)
            p->mostrar();
        cout << "Tiempo total: " << calcularTiempoTotal() << " minutos\n";
    }
};

// Clase del banco de preguntas que gestiona las preguntas totales disponibles/creadas
class BancoPreguntas {
private:
    vector<unique_ptr<Pregunta>> preguntas; // Puntero para almacenar las preguntas
    int nextId = 1; // Id incremental


public:
    void agregarPregunta(unique_ptr<Pregunta> p) {
        preguntas.push_back(move(p));
        nextId++;
    }

    void mostrarTodas() const {
        for (const auto& p : preguntas)
            p->mostrar();
    }

    void eliminarPregunta(int id) {
        for (auto it = preguntas.begin(); it != preguntas.end(); ++it) {
            if ((*it)->getId() == id) {
                preguntas.erase(it);
                cout << "Pregunta eliminada.\n";
                return;
            }
        }
        cout << "No se encontro una pregunta con ese Id.\n";
    }

    void buscarPorNivel(const string& nivel) const {
        cout << "\nPreguntas del nivel \"" << nivel << "\":\n";
        for (const auto& p : preguntas) {
            if (p->getNivel() == nivel)
                p->mostrar();
        }
    }

    Pregunta* obtenerPreguntaPorID(int id) const {
        for (const auto& p : preguntas) {
            if (p->getId() == id)
                return p.get();
        }
        return nullptr;
    }

    // Retorna una lista de preguntas en base a nivel y año
    vector<Pregunta*> obtenerPreguntasPorNivel(string nivel, int cantidad, int anioActual) {
        vector<Pregunta*> seleccionadas;
        for (const auto& p : preguntas) {
            if (p->getNivel() == nivel && p->getAnio() != anioActual - 1) {
                seleccionadas.push_back(p.get());
                if (seleccionadas.size() >= cantidad) break;
            }
        }
        return seleccionadas;
    }

    int getNextId() const { return nextId; }
};

// Esta funcion toma los niveles de Bloom a tiempo, en minutos
int obtenerTiempoPorNivel(string nivel) {
    if (nivel == "Recordar") return 1; // 1 minuto
    if (nivel == "Entender") return 2; // 2 minutos
    if (nivel == "Aplicar") return 3; // 3 minutos
    if (nivel == "Analizar") return 4; // 4 minutos
    if (nivel == "Evaluar") return 5; // 5 minutos
    if (nivel == "Crear") return 6; // 6 minutos
    return 2;
}

// Mostar menu principal
void mostrarMenu() {
    cout << "\n****** Main Menu ******\n";
    cout << "1. Agregar pregunta (Opcion multiple)\n";
    cout << "2. Agregar pregunta (Verdadero/Falso)\n";
    cout << "3. Mostrar todas las preguntas\n";
    cout << "4. Buscar preguntas por nivel\n";
    cout << "5. Eliminar pregunta por Id\n";
    cout << "6. Crear evaluacion\n";
    cout << "0. Salir\n";
    cout << "Seleccione una opcion: ";
}

int main() { // Funcion principal 
    BancoPreguntas banco; // Instancia del branco de preguntas
    int opcion;
    int anioActual = 2025;

    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore(); // Limpieza de buffer, si no el getline saltaria la entrada y quedaria vacia

        if (opcion == 1) {
            string enun, nivel, solucion;
            int numOpciones, correcta;
            vector<string> opciones;

            cout << "Enunciado: ";
            getline(cin, enun);
            cout << "Nivel taxonomico(Recordar, Entender, Aplicar, Analizar, Evaluar, Crear): ";
            getline(cin, nivel);
            cout << "Solucion: ";
            getline(cin, solucion);
            cout << "Cantidad de opciones: ";
            cin >> numOpciones;
            cin.ignore();
            for (int i = 0; i < numOpciones; ++i) {
                string op;
                cout << "Opcion" << i + 1 << ": ";
                getline(cin, op);
                opciones.push_back(op);
            }
            cout << "Numero de opcion correcta (1 a " << numOpciones << "): ";
            cin >> correcta;
            int tiempo = obtenerTiempoPorNivel(nivel);
            banco.agregarPregunta(make_unique<PreguntaOpcionMultiple>(
                banco.getNextId(), enun, nivel, tiempo, solucion, anioActual, opciones, correcta - 1));
            cout << "Pregunta agregada.\n";
        }

        else if (opcion == 2) {
            string enun, nivel, solucion;
            bool respuesta;
            cout << "Enunciado: ";
            getline(cin, enun);
            cout << "Nivel taxonomico(Recordar, Entender, Aplicar, Analizar, Evaluar, Crear): ";
            getline(cin, nivel);
            cout << "Solucion esperada (Verdadero/Falso): ";
            getline(cin, solucion);
            respuesta = (solucion == "Verdadero" || solucion == "verdadero");
            int tiempo = obtenerTiempoPorNivel(nivel);
            banco.agregarPregunta(make_unique<PreguntaVerdaderoFalso>(
                banco.getNextId(), enun, nivel, tiempo, solucion, anioActual, respuesta));
            cout << "Pregunta agregada.\n";
        }

        else if (opcion == 3) { 
            banco.mostrarTodas(); // lista de preguntas
        }

        else if (opcion == 4) {
            string nivel;
            cout << "Ingrese nivel taxonomico a buscar(Recordar, Entender, Aplicar, Analizar, Evaluar, Crear): ";
            getline(cin, nivel);
            banco.buscarPorNivel(nivel);
        }

        else if (opcion == 5) {
            int id;
            cout << "Ingrese Id de pregunta a eliminar: ";
            cin >> id;
            banco.eliminarPregunta(id);
        }

        else if (opcion == 6) {
            string asignatura, nivel;
            int cantidad;
            cout << "Asignatura: ";
            getline(cin >> ws, asignatura);
            cout << "Nivel taxonomico(Recordar, Entender, Aplicar, Analizar, Evaluar, Crear): ";
            getline(cin, nivel);
            cout << "Cantidad de preguntas: ";
            cin >> cantidad;
            Evaluacion eval(asignatura);
            auto seleccionadas = banco.obtenerPreguntasPorNivel(nivel, cantidad, anioActual);
            for (auto p : seleccionadas)
                eval.agregarPregunta(p);
            eval.mostrarEvaluacion(); // Muestra un resumen y tiempo total
        }

    } while (opcion != 0);

    cout << "Programa finalizado.\n";
    return 0;
}

// :D