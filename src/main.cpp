#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cfloat> 

//librerias del grafo
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <random>
#include <queue>

//new
//librerias de opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <random>

#include <map>
#include <set>
//estructura del grafo
using namespace std;

class Arista;

int n = 20;
class Nodo {
public:
  int id;
  float x;
  float y;
  float radius;
  float distancia_euc;
  float distancia_euristica;
  float pesos_acumulados = 0.0;
  int distancia=0;
  vector<Arista*> aristas;
  //para graficar
  GLuint VAO,VBO;
  float color[3]; // Componentes RGB del color
  GLuint shaderProgram; // Programa de shaders
  // Constructor
  Nodo(float x, float y, float radius, float r, float g, float b ,int id,float distancia_euc,float distancia_eursitica) : id(id),x(x), y(y), radius(radius), VAO(0), VBO(0), shaderProgram(0),distancia_euc(distancia_euc),distancia_euristica(distancia_euristica) {
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
  }
  float heuristic(Nodo* goal) {
    return sqrt(pow(x - goal->x, 2) + pow(y - goal->y, 2));
  }
};
        

class Arista {
public:
  Nodo* origen; // Puntero al nodo origen de la arista
  Nodo* destino; // Puntero al nodo destino de la arista
  float peso; // Peso de la arista
  GLuint VAO_line; // ID del Vertex Array Object para la arista
  GLuint VBO_line; // ID del Vertex Buffer Object para la arista
  GLuint shaderProgram; // ID del programa de shaders para la arista
  float color[3]; // Componentes RGB del color de la arista

  // Constructor
  Arista(Nodo* origen, Nodo* destino, float peso, float r, float g, float b) :
    origen(origen), destino(destino), peso(peso), VAO_line(0), VBO_line(0), shaderProgram(0) {
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0

    origen->aristas.push_back(this);
    destino->aristas.push_back(this);
  }
};

class Rectangles{
  public:
  float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;

  unsigned int VAO_rect; // ID del Vertex Array Object para el rect
  unsigned int VBO_rect; // ID del Vertex Buffer Object para el rect
  float color[3]; // Componentes RGB del color del rect

  // Constructor
  Rectangles(float x1, float x2,float x3,float x4,
  float y1, float y2,float y3,float y4,
  float z1, float z2,float z3,float z4,
  float r, float g, float b) :
    x1(x1), x2(x2), x3(x3),x4(x4),
    y1(y1), y2(y2), y3(y3),y4(y4),
    z1(y1), z2(z2), z3(y3),z4(z4),

    VAO_rect(0), VBO_rect(0){
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
  }
};

//compileShaders para nodos
void compileShadersNodo(Nodo& nodo) {
  const char* vertexShaderSource = R"glsl(
  #version 330 core
  layout (location = 0) in vec2 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  }
  )glsl";

  const char* fragmentShaderSource = R"glsl(
  #version 330 core
  out vec4 FragColor;
  uniform vec3 nodeColor; // Color del nodo
  void main() {
    FragColor = vec4(nodeColor, 1.0); // Usar el color del nodo
  }
  )glsl";

  GLuint vertexShader, fragmentShader;
  GLint success;
  GLchar infoLog[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
  }

  nodo.shaderProgram = glCreateProgram();
  glAttachShader(nodo.shaderProgram, vertexShader);
  glAttachShader(nodo.shaderProgram, fragmentShader);
  glLinkProgram(nodo.shaderProgram);
  glGetProgramiv(nodo.shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(nodo.shaderProgram, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

//compileShaders para aristas
void compileShadersArista(Arista& arista) {
  const char* vertexShaderSource = R"glsl(
  #version 330 core
  layout (location = 0) in vec2 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  }
  )glsl";

  const char* fragmentShaderSource = R"glsl(
  #version 330 core
  out vec4 FragColor;
  uniform vec3 edgeColor; // Color de la arista
  void main() {
    FragColor = vec4(edgeColor, 1.0); // Usar el color de la arista
  }
  )glsl";

  GLuint vertexShader, fragmentShader;
  GLint success;
  GLchar infoLog[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
  }

  arista.shaderProgram = glCreateProgram();
  glAttachShader(arista.shaderProgram, vertexShader);
  glAttachShader(arista.shaderProgram, fragmentShader);
  glLinkProgram(arista.shaderProgram);
  glGetProgramiv(arista.shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(arista.shaderProgram, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void createCircle(Nodo& nodo) {
  glGenVertexArrays(1, &nodo.VAO);
  glGenBuffers(1, &nodo.VBO);

  glBindVertexArray(nodo.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, nodo.VBO);

  float vertices[200];
  float radius = nodo.radius;
  for (int i = 0; i < 100; ++i) {
    float theta = 2.0f * 3.1415926f * float(i) / float(100);
    vertices[i * 2] = radius * cosf(theta) + nodo.x;
    vertices[i * 2 + 1] = radius * sinf(theta) + nodo.y;
  }

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void createEdge(Arista& arista) {
  glGenVertexArrays(1, &(arista.VAO_line));
  glGenBuffers(1, &(arista.VBO_line));

  glBindVertexArray(arista.VAO_line);
  glBindBuffer(GL_ARRAY_BUFFER, arista.VBO_line);

  float lineVertices[] = {
    arista.origen->x, arista.origen->y,
    arista.destino->x, arista.destino->y
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void createRectangles(Rectangles& rectangles) {
  float vertices[] = {
    rectangles.x3,rectangles.y3,rectangles.z3,
    rectangles.x4,rectangles.y4,rectangles.z4,
    rectangles.x2,rectangles.y2,rectangles.z2,
    rectangles.x1,rectangles.y1,rectangles.z1,
  };

  glGenVertexArrays(1, &rectangles.VAO_rect);
  glBindVertexArray(rectangles.VAO_rect);

  glGenBuffers(1, &rectangles.VBO_rect);
  glBindBuffer(GL_ARRAY_BUFFER, rectangles.VBO_rect);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


class Grafo {
public:
  vector<Nodo*> nodos;
  vector<Arista*> aristas;

  Grafo() {
    float posx = -0.95f;
    float posy = 0.85f;
    for (int i = 0; i < 21 * 21; i++)
    {
      agregarNodo(posx,posy,0.02f,255.0f,255.0f,255.0f,i);
      posx+=0.08;
      if ((i + 1) % 21 == 0)
      {
        posy-= 0.08;
        posx= -0.95f;
      }
    }

    // Conectar nodos con aristas
    for (int i = 0; i < (n + 1) * (n + 1); i++) {
      int row = i / (n + 1);
      int col = i % (n + 1);

      // Conectar con nodo a la derecha
      if (col < n) {
        agregarArista(i, i + 1, 10);
      }

      // Conectar con nodo abajo
      if (row < n) {
        agregarArista(i, i + (n + 1), 10);

        // Conectar con nodo abajo izquierda
        if (col > 0) {
        agregarArista(i, i + (n + 1) - 1, 14.14f);
        }

        // Conectar con nodo abajo derecha
        if (col < n) {
        agregarArista(i, i + (n + 1) + 1, 14.14f);
        }
      }
    }
  }

  ~Grafo() {
    for (Nodo* nodo : nodos)
      delete nodo;

    for (Arista* arista : aristas)
      delete arista;
  }

  void agregarNodo(float x, float y, float radius, float r, float g, float b ,int id) {
    nodos.push_back(new Nodo(x, y, radius, r, g, b,id,0.0,0.0));
  }

  void agregarArista(int idOrigen, int idDestino, float peso) { // Cambiado a float
    Nodo* origen = nullptr;
    Nodo* destino = nullptr;

    for (Nodo* nodo : nodos) {
      if (nodo->id == idOrigen)
        origen = nodo;
      if (nodo->id == idDestino)
        destino = nodo;
    }

    if (origen && destino) {
      Arista* x = new Arista(origen, destino, peso, 0, 128, 0);
      aristas.push_back(x);
    }
  }

  void borrarNodo(int idNodo) {
    // Buscar el nodo a eliminar
    Nodo* nodoEliminar = nullptr;
    for (Nodo* nodo : nodos) {
      if (nodo->id == idNodo) {
        nodoEliminar = nodo;
        break;
      }
    }

    if (!nodoEliminar)
      return;

    // Eliminar las aristas conectadas al nodo a eliminar y eliminarlas de VectorAristas
    for (Arista* arista : nodoEliminar->aristas) {
      auto& vecinoAristas = arista->destino == nodoEliminar ? arista->origen->aristas : arista->destino->aristas;
      vecinoAristas.erase(remove(vecinoAristas.begin(), vecinoAristas.end(), arista), vecinoAristas.end());
      aristas.erase(remove(aristas.begin(), aristas.end(), arista), aristas.end());
      delete arista;
    }

    // Eliminar el nodo
    nodos.erase(remove(nodos.begin(), nodos.end(), nodoEliminar), nodos.end());
    delete nodoEliminar;
  }

  void imprimir() {
    // Imprimir los nodos en orden
    for (int i = 0; i < nodos.size(); i++) {
      createCircle(*nodos[i]);
      compileShadersNodo(*nodos[i]);
    }

    // Imprimir las aristas
    for (int i = 0; i < aristas.size(); i++) {
      createEdge(*aristas[i]);
      compileShadersArista(*aristas[i]);
    }
  }

  void randomDelete(int a) {
    if (a < 0) a = 0;
    if (a > 100) a = 100;

    random_device rd;
    mt19937 g(rd());

    int numNodosEliminar = (a * nodos.size()) / 100;

    vector<int> indicesNodos(nodos.size());
      for (int i = 0; i < indicesNodos.size(); ++i) {
      indicesNodos[i] = i;
    }

    // Mezclar los índices de forma aleatoria
    shuffle(indicesNodos.begin(), indicesNodos.end(), g);

    // Lista temporal para almacenar los nodos a eliminar
    vector<Nodo*> nodosAEliminar;

    // Agregar los nodos a eliminar a la lista temporal
    for (int i = 0; i < numNodosEliminar; ++i) {
      int idNodoEliminar = nodos[indicesNodos[i]]->id;
      for (auto iter = nodos.begin(); iter != nodos.end(); ++iter) {
        if ((*iter)->id == idNodoEliminar) {
          nodosAEliminar.push_back(*iter);
          break;
        }
      }
    }

    for (auto nodo : nodosAEliminar) {
      borrarNodo(nodo->id);
    }
  }

  //BUSQUEDA PROFUNDIDAD

  // void DFS(int razon, int direccion, int id_inicio, int id_destino){
  //   for (vector<Nodo* >::iterator it = nodos.begin(); it < nodos.end(); it++) {
  //     Nodo* nodo_actual = *it;
  //     int i = nodo_actual->id;
  //     if(i == id_inicio || i == id_destino){

  //       nodo_actual->color[0] = 1.0f;
  //       nodo_actual->color[1] = 1.0f;
  //       nodo_actual->color[2] = 0.0f;
  //     }            
  //     createCircle(*nodos[i]);
  //     compileShadersNodo(*nodos[i]);
  //   }
  //   for (int i = 0; i < aristas.size(); i++) {
  //     createEdge(*aristas[i]);
  //     compileShadersArista(*aristas[i]);
  //   }
  //   //for de nodos buscando con razon y direccion 
  //   //for if !not visited - function (Change color)            
  // }

  int profundidad(int nodoInicial, int nodoFinal) {
    vector<Nodo*> road;
    int recorrido = 0;

    for (int i=0; i < nodos.size(); i++){
        if(nodoInicial==nodos[i]->id){
            road.insert(road.begin(), nodos[i]);
            break;
        }
    }

    while (!road.empty()) {
      Nodo* nodoActual = road.front();
      road.erase(road.begin());
      
      nodoActual->color[0] = 255.0f;
      nodoActual->color[1] = 0;
      nodoActual->color[2] = 0;

      // cout<<"ID: "<<nodoActual->id<<endl;
      if (nodoActual->id == nodoFinal) {
        // cout << "Done" << endl;
        return recorrido;
      }

      for (int i = 0; i < nodoActual->aristas.size(); i++) {
        Nodo* siguiente = (nodoActual->aristas[i]->origen == nodoActual) ? nodoActual->aristas[i]->destino : nodoActual->aristas[i]->origen;
        if (siguiente->color[0] == 0.0f || siguiente->color[0] == 1.0f) {
          siguiente->color[0] = 255.0f;
          siguiente->color[1] = 0.0f;
          siguiente->color[2] = 0.0f;
          siguiente->distancia = nodoActual->id;
          road.insert(road.begin(),siguiente);
          recorrido += aristas[i]->peso;
          // cout<<"Siguiente: " << road.front()->id<<"\t";
        }
      }
      // cout<<endl;
    }

    cout << "No se encontro camino" << endl;
    return recorrido;
  }

  void profundidadCaminoCorto(int nodoInicial, int nodoFinal){
    if(nodoFinal==nodoInicial) return;
    for (int i=0; i < nodos.size(); i++){
      if(nodoFinal == nodos[i]->id){
        nodos[i]->color[0]=0.0f;
        nodos[i]->color[1]=1.0f;
        nodos[i]->color[2]=0.0f;
        if(nodos[i]->distancia == -1) return;
        profundidadCaminoCorto(nodoInicial,nodos[i]->distancia);
        break;
      }
    }    
  }

    bool findNodo(int id){
    for (int i = 0; i < nodos.size(); i++) {
      if (nodos[i]->id == id) return true;
    }
    return false;
  }

  // BUSQUEDA POR AMPLITUD
  bool amplitud(int nodoInicial, int nodoFinal){
    queue<Nodo*> cola;
    float recorrido=0;

    for (int i=0; i < nodos.size(); i++){
      if(nodoInicial==nodos[i]->id){
        cola.push(nodos[i]);
            nodos[i]->color[0] = 1.0f;
            nodos[i]->color[1] = 0.0f;
            nodos[i]->color[2] = 0.0f;
        break;
      }
    }

    while(!cola.empty()){
      Nodo* actual = cola.front();
      cola.pop();
      // cout<<actual->id<<endl;

      if(actual->aristas.empty()){
        cout<<"No se encontro el nodo "<<nodoFinal<<endl;
        return false;
      }

      if (actual->id == nodoFinal) {
        actual->color[1] = 0.0f;
        actual->color[2] = 0.0f;
        // cout<<actual->id<<" "<<actual->distancia<<endl;
        // cout<<"Se encontro el nodo "<<nodoFinal<<endl;
        cout<<"Distancia recorrida: "<<recorrido<<endl;
        return true;
      }

      for(int i=0; i<actual->aristas.size();i++){
        if(actual->id==actual->aristas[i]->origen->id && actual->aristas[i]->destino->color[1]!=0.0f){
          cola.push(actual->aristas[i]->destino);
          actual->aristas[i]->destino->color[1]=0.0f;
          actual->aristas[i]->destino->color[2]=0.0f;
          actual->aristas[i]->destino->distancia=actual->id;
          recorrido+=actual->aristas[i]->peso;
          // cout<<actual->id<<" "<<actual->distancia<<endl;
          // cout<<actual->aristas[i]->destino->id<<" ";
        }

        if(actual->id==actual->aristas[i]->destino->id && actual->aristas[i]->origen->color[1]!=0.0f){
          cola.push(actual->aristas[i]->origen);
          actual->aristas[i]->origen->color[1]=0.0f;
          actual->aristas[i]->origen->color[2]=0.0f;
          actual->aristas[i]->origen->distancia=actual->id;
          recorrido+=actual->aristas[i]->peso;
          // cout<<actual->id<<" "<<actual->distancia<<endl;
          // cout<<actual->aristas[i]->origen->id<<" ";
        }
      }
    }
    cout<<"No se encontro el nodo "<<nodoFinal<<endl;
    return false;
  }

  void amplitudCaminoCorto(int nodoInicial, int nodoFinal){
    int recorrido = 0;
    if(nodoFinal==nodoInicial){
      for (int j=0; j < nodos.size(); j++){
        if(nodoFinal==nodos[j]->id){
          nodos[j]->color[0]=0.0f;
          nodos[j]->color[1]=1.0f;
          nodos[j]->color[2]=0.0f;
          return;
          // cout<<nodos[i]->id<<" "<<nodos[i]->distancia;
        }
      }
    } 
    for (int i=0; i < nodos.size(); i++){
      if(nodoFinal==nodos[i]->id){
        nodos[i]->color[0]=0.0f;
        nodos[i]->color[1]=1.0f;
        nodos[i]->color[2]=0.0f;
        // cout<<nodos[i]->id<<" "<<nodos[i]->distancia;
        amplitudCaminoCorto(nodoInicial,nodos[i]->distancia);
      }
    }    
  }

  //BUSQUEDA HILLCLIMBING
  float hillClimbing(int idOrigen, int idDestino, vector<Nodo*>& resp)
  {
    Nodo* a = nullptr;
    Nodo* b = nullptr;

    for (Nodo* nodo : nodos) {
      if (nodo->id == idOrigen) {
        a = nodo;
      }
      if (nodo->id == idDestino) {
        b = nodo;
      }
    }

    if (!a || !b) {
      return -1;
    }

    map<Nodo*, int> distance;
    map<Nodo*, Nodo*> previous;
    set<Nodo*> visited;
    priority_queue<pair<float, Nodo*>, vector<pair<float, Nodo*>>, greater<pair<float, Nodo*>>> nodeQueue;

    for (Nodo* node : nodos) {
      distance[node] = INT_MAX;
    }

    distance[a] = 0;
    nodeQueue.push({a->heuristic(b), a});

    while (!nodeQueue.empty()) {
      Nodo* current = nodeQueue.top().second; // Obtener el nodo actual de la cola de prioridad
      nodeQueue.pop(); // Eliminar el nodo actual de la cola de prioridad

      if (current == b) {
        break; // Si hemos llegado al nodo objetivo, salir del bucle
      }

      visited.insert(current); // Marcar el nodo actual como visitado
      current->color[0] = 1.0f;
      current->color[1] = 0.1f;
      current->color[2] = 0.1f;

      for (Arista* edge : current->aristas) {
        Nodo* neighbor = (edge->origen == current) ? edge->destino : edge->origen; // Obtener el vecino del nodo actual
        neighbor->color[0] = 1.0f;
        neighbor->color[1] = 0.1f;
        neighbor->color[2] = 0.0f;
        // Calcular la distancia euclidiana del vecino al nodo objetivo
        float heuristic = neighbor->heuristic(b);

        // Si el vecino no ha sido visitado y su distancia euclidiana es menor que la de otros vecinos
        if (visited.find(neighbor) == visited.end()) {
          distance[neighbor] = heuristic; // Actualizar la distancia (en este caso, solo la distancia euclidiana)
          previous[neighbor] = current; // Actualizar el nodo anterior del vecino
          nodeQueue.push({heuristic, neighbor}); // Agregar el vecino a la cola de prioridad
        }
      }
    }

    if (distance[b] == INT_MAX) {
      return -1; // Si no se encontró un camino al nodo objetivo, retornar -1
    }

    // Reconstruir el camino desde el nodo objetivo hasta el nodo de inicio
    Nodo* current = b;
    while (current != a) {
      resp.push_back(current);
      current->color[0] = 0.1f;
      current->color[1] = 1.0f;
      current->color[2] = 0.1f;
      current = previous[current];
    }
    resp.push_back(a);
    reverse(resp.begin(), resp.end()); // Invertir el vector de respuesta para obtener el camino correcto

    float totalRecorrido = 0;
    for (int i = 0; i < resp.size(); i++) {
      for (Arista* arista : resp[i]->aristas) {
        if (arista->destino == resp[i+1] || arista->origen == resp[i+1]) {
          totalRecorrido = totalRecorrido + arista->peso;
          break;
        }
      }
    } // Retornar la distancia total desde el nodo de inicio hasta el nodo objetivo
    return totalRecorrido; // Retornar la distancia total desde el nodo de inicio hasta el nodo objetivo
  }
};

//estructura del graficador
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// variables globales
Grafo G;
vector<Nodo*> inicio_fin;

vector<Nodo*> OrdenarNodos(vector<Nodo*>& vector_Ord)
{
  // Ordenar el vector de nodos según el peso
  sort(vector_Ord.begin(), vector_Ord.end(), [](Nodo* a, Nodo* b) {
    for(int i =0;i<a->aristas.size();i++)
    {
      if(a->aristas[i]->destino == inicio_fin[0])
      {
        for(int j = 0;j<b->aristas.size();j++)
        {
          if(b->aristas[j]->destino == inicio_fin[0])
          {
              return a->aristas[i]->peso < b->aristas[j]->peso; // Comparar por peso de la primera arista de cada nodo destino
          }
        }
      }
    }
    //warning pero nunca llega
    return a->aristas[0]->peso < b->aristas[0]->peso; // Comparar por peso de la primera arista de cada nodo destino
});
    return vector_Ord;
}

vector<Nodo*> nodos_cercanos(vector<Nodo*>& vector_cercanos,Nodo nodo_central)
{
  for(int i = 0;i<nodo_central.aristas.size();i++)
  {
    vector_cercanos.push_back(nodo_central.aristas[i]->destino);
    //cout<<"Id nodos colindantes: "<<nodo_central.aristas[i]->destino->id<<endl;
  }
  return vector_cercanos;
}


float calcularDistanciaEuc(Nodo* a, Nodo* b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  return sqrt(dx * dx + dy * dy);
}

float calcularDistanciaHeuristica(Nodo* a, Nodo* b) {
  return calcularDistanciaEuc(a, b);
}

vector<Nodo*> A_asterisco() {

    int count_nodos = 0;

    //coordenadas del destino
    float destinox = ( inicio_fin[1]->x/ 800) * 2 - 1;
    float destinoy =  1 - ((inicio_fin[1]->y/800)*2);

    // distancia euclidiana
    for(int i = 0;i<G.nodos.size();i++)
    {    
        float normalizedX = (G.nodos[i]->x / 800) * 2 - 1; // Convertir de rango [0, screenWidth] a [-1, 1]
        float normalizedY =  1 - ((G.nodos[i]->y/800)*2); // Convertir de rango [0, screenHeight] a [-1, 1] 
        float distancia = sqrt(pow(destinox - normalizedX, 2) + pow(destinoy - normalizedY, 2) );
        G.nodos[i]->distancia_euc = distancia*100000;
    }

    std::map<int, int> mapa; //mapa para nodos tomados
    mapa.insert(std::make_pair(inicio_fin[0]->id,count_nodos ));
    count_nodos++;
 
    Nodo* inicio = inicio_fin[0];
    Nodo* destino = inicio_fin[1];
    
    vector<Nodo*> cola_camino;
    cola_camino.push_back(inicio);
    float menor_acumulado;
    menor_acumulado = std::numeric_limits<float>::max();
    int id_menor_acumulado = 0;
    int peso_arista_menor_acumulado = 0;
    int activa_back = 1;

    while(cola_camino[cola_camino.size()-1]->id != inicio_fin[1]->id && !cola_camino.empty())
    {
        menor_acumulado = std::numeric_limits<float>::max();
        id_menor_acumulado = 0;
        peso_arista_menor_acumulado = 0;
        activa_back = 1;
        for(int i = 0;i<cola_camino[cola_camino.size()-1]->aristas.size();i++)
        {
            if(cola_camino[cola_camino.size()-1]->aristas[i]->destino->id ==cola_camino[cola_camino.size()-1]->id )
            {
                cola_camino[cola_camino.size()-1]->aristas[i]->origen->color[0] = 1.0f;
                cola_camino[cola_camino.size()-1]->aristas[i]->origen->color[1] = 0.1f;
                cola_camino[cola_camino.size()-1]->aristas[i]->origen->color[2] = 0.1f;

                 // Acceder directamente al valor a través de la clave
                if (mapa.count(cola_camino[cola_camino.size()-1]->aristas[i]->origen->id) > 0) {
                    //std::cout << "Valor encontrado: " << cola_camino[cola_camino.size()-1]->aristas[i]->origen->id << std::endl;
                   
                }
                else
                {   if(cola_camino[cola_camino.size()-1]->pesos_acumulados 
                    + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                    + cola_camino[cola_camino.size()-1]->aristas[i]->origen->distancia_euc
                    <=menor_acumulado)
                    {
                        menor_acumulado = cola_camino[cola_camino.size()-1]->pesos_acumulados 
                    + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                    + cola_camino[cola_camino.size()-1]->aristas[i]->origen->distancia_euc;

                    id_menor_acumulado = i;
                    peso_arista_menor_acumulado = cola_camino[cola_camino.size()-1]->aristas[i]->peso;
                    activa_back = 0;
                    }

                    //cout<<"ID REVISADO: "<<cola_camino[cola_camino.size()-1]->aristas[i]->origen->id<<"Distancia euristica: "<<cola_camino[cola_camino.size()-1]->pesos_acumulados 
                   // + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                    //+ cola_camino[cola_camino.size()-1]->aristas[i]->origen->distancia_euc<<endl;;
                }

            }
            else if(cola_camino[cola_camino.size()-1]->aristas[i]->origen->id ==cola_camino[cola_camino.size()-1]->id) {
                cola_camino[cola_camino.size()-1]->aristas[i]->destino->color[0] = 1.0f;
                cola_camino[cola_camino.size()-1]->aristas[i]->destino->color[1] = 0.1f;
                cola_camino[cola_camino.size()-1]->aristas[i]->destino->color[2] = 0.1f;

                  // Acceder directamente al valor a través de la clave
                if (mapa.count(cola_camino[cola_camino.size()-1]->aristas[i]->destino->id) > 0) {
                    //std::cout << "Valor encontrado: " << cola_camino[cola_camino.size()-1]->aristas[i]->destino->id << std::endl;
                    continue;
                }

                if(cola_camino[cola_camino.size()-1]->pesos_acumulados 
                + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                + cola_camino[cola_camino.size()-1]->aristas[i]->destino->distancia_euc
                <=menor_acumulado)
                {
                    menor_acumulado = cola_camino[cola_camino.size()-1]->pesos_acumulados 
                + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                + cola_camino[cola_camino.size()-1]->aristas[i]->destino->distancia_euc;

                id_menor_acumulado = i;
                peso_arista_menor_acumulado = cola_camino[cola_camino.size()-1]->aristas[i]->peso;
                activa_back = 0;
                
                }
                //cout<<"ID REVISADO: "<<cola_camino[cola_camino.size()-1]->aristas[i]->destino->id<<"Distancia euristica: "<<cola_camino[cola_camino.size()-1]->pesos_acumulados 
                //    + cola_camino[cola_camino.size()-1]->aristas[i]->peso 
                //    + cola_camino[cola_camino.size()-1]->aristas[i]->destino->distancia_euc<<endl;;
            }
        }

        //cout<<"Menor Acumulado:"<<menor_acumulado<<endl;
        if(activa_back == 1)
        {
            cola_camino.back()->color[0] = 1.0f;
          cola_camino.back()->color[1] = 0.1f;
          cola_camino.back()->color[2] = 0.1f;
            cola_camino.pop_back();
            if(cola_camino.empty())
            {break;}
            activa_back = 0;
        }
        else{

            if(cola_camino[cola_camino.size()-1]->aristas[id_menor_acumulado]->destino->id == 
            cola_camino[cola_camino.size()-1]->id)
            {
                Nodo* auxiliar = cola_camino.back();
                //cola_camino[cola_camino.size()-1]->aristas[id_menor_acumulado]->origen->color[0] = 255;
                cola_camino.push_back(cola_camino[cola_camino.size()-1]->aristas[id_menor_acumulado]->origen);
                cola_camino.back()->pesos_acumulados = peso_arista_menor_acumulado+cola_camino.back()->pesos_acumulados+ auxiliar->pesos_acumulados;
                if (cola_camino.back()->id == inicio_fin[1]->id) break;
                
                mapa.insert(std::make_pair(auxiliar->aristas[id_menor_acumulado]->origen->id,count_nodos ));
                count_nodos++;

            }
            else{
                
                Nodo* auxiliar = cola_camino.back();
                //cola_camino[cola_camino.size()-1]->aristas[id_menor_acumulado]->destino->color[0] = 255;
                cola_camino.push_back(cola_camino[cola_camino.size()-1]->aristas[id_menor_acumulado]->destino);
                cola_camino.back()->pesos_acumulados = peso_arista_menor_acumulado+cola_camino.back()->pesos_acumulados+ auxiliar->pesos_acumulados;
                if (cola_camino.back()->id == inicio_fin[1]->id) break;
                
                mapa.insert(std::make_pair(auxiliar->aristas[id_menor_acumulado]->destino->id,count_nodos ));
                count_nodos++;
            }
        }
    }
    return cola_camino;
}
//PROFUNDIDAD
void getInicioDestino(Grafo &G, int &inicio, int &destino){
  char conf;
  while(1){
    cout<<"Ingrese el inicio y final\n";
    cout<<"Inicio: ";cin>>inicio;

    while(!G.findNodo(inicio)) {
      cout<<"Nodo no encontrado.\n";
      cout<<"Inicio: ";cin>>inicio;
    }

    cout<<"Destino: ";cin>>destino;

    while(!G.findNodo(destino)) {
      cout<<"Nodo no encontrado.\n";
      cout<<"Destino: ";cin>>destino;
    }
    
    cout<<"Seguro? y/N: ";cin>>conf;
    if(conf == 'y') return;
  }
}
void resetGrafo () {
  for (Nodo* nodo : G.nodos) {
    nodo->color[0] = 1.0f;
    nodo->color[1] = 1.0f;
    nodo->color[2] = 1.0f;
    nodo->distancia_euc = 0.0;
    nodo->distancia_euristica = 0.0;
    nodo->pesos_acumulados = 0.0;
    nodo->distancia = 0;
    inicio_fin.clear();
  }
}
// Función para manejar los eventos de click
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
  // Verificar si el botón izquierdo del mouse ha sido presionado
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Imprimir las coordenadas del click
    // cout << "Click en coordenadas (" << xpos << ", " << ypos << ")" << endl;
    // Obtener las coordenadas normalizadas del cursor del mouse
    float normalizedX = (xpos / 800) * 2 - 1; // Convertir de rango [0, screenWidth] a [-1, 1]
    float normalizedY =  1 - ((ypos/800)*2); // Convertir de rango [0, screenHeight] a [-1, 1]
    // cout << "Click en coordenadas normalizadas (" << normalizedX << ", " << normalizedY << ")" << endl;

    // cout << "Coordenadas del primer Nodo(" << G.nodos[0]->x << ", " << G.nodos[0]->y << ")" << endl;
    for (int i = 0; i<G.nodos.size() ; i++) {
      if (pow(normalizedX - G.nodos[i]->x, 2) + pow(normalizedY - G.nodos[i]->y, 2) <= pow(G.nodos[i]->radius, 2)) {
        // Cambiar el color del nodo encontrado a rojo
        G.nodos[i]->color[0] = 0;
        G.nodos[i]->color[1] = 255;
        G.nodos[i]->color[2] = 0;
        // cout<<"Nodo encontrado: "<<G.nodos[i]->id<<endl;
        // cout<<"Nodo encontrado pos x: "<<G.nodos[i]->x<<endl;
        // cout<<"Nodo encontrado pos y: "<<G.nodos[i]->y<<endl;
        inicio_fin.push_back(G.nodos[i]);
        break; // Salir del bucle al encontrar el nodo
      }
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.88f && normalizedY > 0.75f)
    {           
      //A_ASTERISCO
      cout<<endl;
      cout<<"A*"<<endl;
      vector<Nodo*> camino = A_asterisco();
      if(!camino.empty())
      {
        //cout << "Camino: ";
        for(int i =0;i<camino.size();i++)
        {
          //cout<<camino[i]->id<<' ';
          camino[i]->color[0] = 0.1f;
          camino[i]->color[1] = 1.0f;
          camino[i]->color[2] = 0.1f;
        }
        cout << endl;
        cout << "Distancia recorrida: " << camino[camino.size() - 1]->pesos_acumulados << endl;
      }
      else{
        cout<<"Camino no encontrado: "<<endl;
      }
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.68f && normalizedY > 0.55f)
    {           
      //AMPLITUD
      cout<<endl;
      cout<<"AMPLITUD"<<endl;
      int inicio = inicio_fin[0]->id;
      int destino = inicio_fin[1]->id;
      bool encontrado=G.amplitud(inicio,destino);
      if(encontrado){
        G.amplitudCaminoCorto(inicio,destino);
      }
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.48f && normalizedY > 0.35f)
    {           
      //PROFUNDIDAD
      cout<<endl;
      cout<<"PROFUNDIDAD"<<endl;
      int inicio = inicio_fin[0]->id;
      int destino = inicio_fin[1]->id;
      inicio_fin[1]->distancia = -1;
      int recorrido = G.profundidad(inicio, destino);
      G.profundidadCaminoCorto(inicio, destino);
      // G.DFS(0,0,inicio, destino);
      cout<<endl<<"Distancia recorrida: "<<recorrido<<endl;
    }

    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.28f && normalizedY > 0.15f)
    {
      //HILLCLIMBING
      cout<<endl;
      cout<<"HILL CLIMBING"<<endl;
      vector<Nodo*> resp;
      float d = G.hillClimbing(inicio_fin[0]->id, inicio_fin[1]->id, resp);
      if (d != -1) {
        cout << endl;
        //cout << "Camino: ";
        for ( Nodo* nodo : resp) {
          //cout << nodo->id << ' ';
        }
        cout << endl;
        cout << "Distancia recorrida: " << d << endl;
      }
      else {
        cout << "Camino no encontrado"<<endl;
      }
    }

    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.08f && normalizedY > -0.05f)
    {           
      //REFRESH
      resetGrafo();
    }
  }
}

int main() {
  //main del graficador

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 800, "Grafo", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW Window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  //funcion del mouse
  glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
  mouse_callback(window, button, action, mods);
  });

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }
  //termina de crear la pantalla 

  //glViewport(0, 0, 800, 800);
  //glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

  //main del grafo
  cout << endl;
  //cout << (G.nodos[0])->aristas[0]->peso;
  int porcentaje_eliminar;
  cout<<"Ingrese el porcentaje a Eliminar: "<<endl;
  cin>>porcentaje_eliminar;
  G.randomDelete(porcentaje_eliminar);
  cout<<"GRAFO ACTUALIZADO"<<endl;
  G.imprimir();

  
  vector<Rectangles*> algoritmos_mostrar;
  float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
  x1 =0.7f;
  y1 = 0.88f;
  z1 = 0.0f;

  x2 = 0.99f;
  y2 = 0.88f;
  z2 = 0.0f;

  x3 = 0.7f;
  y3 = 0.75f;
  z3 = 0.0f;

  x4 =  0.99f;
  y4 = 0.75f;
  z4 = 0.0f;
  
  for(int i = 0; i <5;i++)
  {
    algoritmos_mostrar.push_back(new Rectangles(x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4,0,255,0));
    createRectangles(*algoritmos_mostrar[i]);
    y1-=0.2f;
    y2-=0.2f;
    y3-=0.2f;
    y4-=0.2f;
  }
  //cout<<"GRAFO ACTUALIZADO"<<endl;

  while (!glfwWindowShouldClose(window)) {
    // Procesar eventos de la ventana
    glfwPollEvents();
    processInput(window);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // Imprimir los nodos en orden
    for (int i = 0; i < G.nodos.size(); i++) {
      //cout << G.nodos[i]->id << "\t";
      glUseProgram(G.nodos[i]->shaderProgram);
      // Pasa el color del nodo al shader fragment
      int nodeColorLocation = glGetUniformLocation(G.nodos[i]->shaderProgram, "nodeColor");
      glUniform3fv(nodeColorLocation, 1, G.nodos[i]->color);
      glBindVertexArray(G.nodos[i]->VAO);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 100);
    }
    // Dibujar aristas
    // Imprimir las aristas
    for (int i = 0; i < G.aristas.size(); i++) {
      glUseProgram(G.aristas[i]->shaderProgram);
      int edgeColorLocation = glGetUniformLocation(G.aristas[i]->shaderProgram, "edgeColor");
      glUniform3fv(edgeColorLocation, 1, G.aristas[i]->color);
      glBindVertexArray(G.aristas[i]->VAO_line);
      glDrawArrays(GL_LINES, 0, 2);
    }

    //dibuja rectangulos

    for(int i = 0; i<algoritmos_mostrar.size();i++)
    {
      glBindVertexArray(algoritmos_mostrar[i]->VAO_rect);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Modificado para dibujar un rectángulo
    }
    glfwSwapBuffers(window);
  }


  //liberar memoria
  for (int i = 0; i < G.nodos.size(); i++) {
    glDeleteVertexArrays(1, &(G.nodos[i])->VAO);
    glDeleteBuffers(1, &(G.nodos[i])->VBO);
    glDeleteProgram(G.nodos[i]->shaderProgram);
  }
  for (int i = 0; i < G.aristas.size(); i++) {
    glDeleteVertexArrays(1, &(G.aristas[i])->VAO_line);
    glDeleteBuffers(1, &(G.aristas[i])->VBO_line);
    glDeleteProgram(G.aristas[i]->shaderProgram);
  }
  

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}