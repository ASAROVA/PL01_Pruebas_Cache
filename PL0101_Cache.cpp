#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;
#define SL cout<<endl
#define MIN -99
#define MAX 100

/// Generador de Numero Aleatorio /////////////////
#include <ctime>
int aleatorio (int minimo=0, int maximo=RAND_MAX )
{
    static bool inicio = true;
    if (inicio) { srand(time(NULL));
                  inicio = false;    }
    return minimo + rand()%(maximo-minimo);
}
/// ////////////////////////////////////////////*/

/// Matriz ///////////////////////////////////////
template<class T>
class matriz : private vector<vector<T>>
{
protected:
    int nfilas, ncolum;
    typedef vector<vector<T>> vvt;

public:

    using vvt::push_back;
    using vvt::operator[];
    using vvt::begin;
    using vvt::end;

    matriz(): nfilas(0), ncolum(0), vvt() {}
    matriz(initializer_list<vector<T>> L): vvt(L)
    {
        nfilas = L.size();
        ncolum = L.begin()->size();
        cout << "Dim: " << nfilas << ", " << ncolum; SL;SL;
    }
    //virtual ~matriz() { vvt::~vvt(); }

    vector<T> operator* (const vector<T>& X) const
    {
        if(X.size()!=ncolum)
            throw invalid_argument( "Dimensiones incompatibles" );
        vector<T> Y(nfilas,0);
        for(int i=0; i<nfilas; i++)
        {
            for(int j=0; j<ncolum; j++)
                Y[i] += (*this)[i][j]*X[j];
            //cout << Y[i] <<"\t";
        }
    return Y;
    }

    vector<T> producto_ji (const vector<T>& X) const
    {
        if(X.size()!=ncolum)
            throw invalid_argument( "Dimensiones incompatibles" );
        vector<T> Y(nfilas,0);
        for(int j=0; j<ncolum; j++)
        {
            for(int i=0; i<nfilas; i++)
                Y[i] += (*this)[i][j]*X[j];
            //cout << Y[i] <<"\t";
        }
    return Y;
    }

    void rellenar ( int num_filas,  int num_columnas,
                    int minimo=MIN, int maximo=MAX   )
    {
        nfilas = num_filas;
        ncolum = num_columnas;
        this->resize(nfilas);
        for (auto& i: *this)
        {
            i.resize(ncolum);
            for(auto& ij: i) ij = aleatorio(minimo,maximo);
        }
    }

    void imprimir()
    {
        for(int i=0; i<nfilas; i++)
        {
            for(int j=0; j<ncolum; j++)
                cout << (*this)[i][j] << "\t";
            SL;
        }
    }

    template<class TT>
    friend ostream &operator<<(ostream &, const matriz<TT> &);
};

template<class T>
ostream &operator<<(ostream &os, matriz<T> const &M)
{
    for(int i=0; i<M.nfilas; i++)
    {
        for(int j=0; j<M.ncolum; j++)
            os << M[i][j] << "\t";
        os << "\n";
    }
    return os;
}

typedef matriz<int>   matrint;
typedef matriz<float> matflot;
/// ////////////////////////////////////////////*/

/// Definicion del tipo de funcion a medir ///////
typedef int tipo;
typedef vector<int>(matriz<tipo>::*funcion)(const vector<int>&)const;
/// ////////////////////////////////////////////*/

/// Funcion que mide la demora de otra funcion ///
double demora (funcion f, int alto, int ancho=0)
{
    if(!ancho) ancho = alto;
    matriz<tipo> M;
    M.rellenar(alto,ancho);

    vector<tipo> X(ancho,aleatorio(MIN,MAX));

    clock_t t;
    t = clock();
    (M.*f)(X);
    t = clock()-t;
    //return double(t)/CLOCKS_PER_SEC;
    return double(t);
}
/// ////////////////////////////////////////////*/

/// Functor que mide la demora de una función ////
class cronometro
{
    funcion f;
public:
    cronometro() {}
    void est_funcion(funcion F) {f=F;}
    double operator()(int alto, int ancho=0)
    {
        if(!ancho) ancho = alto;
        matriz<tipo> M;
        M.rellenar(alto,ancho);

        vector<tipo> X(ancho,aleatorio(MIN,MAX));

        clock_t t;
        t = clock();
        (M.*f)(X);
        t = clock()-t;
        return double(t)/double(CLOCKS_PER_SEC);
        //return double(t);
    }
};
/// ////////////////////////////////////////////*/

/// Rutina de Tabulacion de vectores /////////////
template <class T, class... TY>
void tabulador(vector<T> X, TY... Ys)
{
    int n = X.size();
    for(int i=0;i<n;i++)
    {
        cout << X[i] << "\t\t";
        for(auto& Y: {Ys...})
            cout << Y[i] << "\t\t";
        SL;
    }
}
/// ////////////////////////////////////////////*/

/// Rellenador de vectores por interpolacion /////
template <class T>
void interpolar(vector<T>& v,T inicio,T incremento,T fin)
{
    v.resize( 1 + (int)(fin-inicio)/(int)incremento );
    T x = inicio;
    for(auto& i:v)
    {
        i = x;
        x += incremento;
    }
}
/// ////////////////////////////////////////////*/



int main(void)
{
    cout << "\n\t\t >> ASAROVA << \n\n";

    /// Usamdo el functor "cronometro" /////////////////

    funcion f;
    vector<int> tamannos {100,200,400,800,1600,3200,6400,12800,25600};
    //vector<int> tamannos {100,200,400,800};
    //vector<int> tamannos; interpolar(tamannos,100,100,2000);

    cout << "Tabla de Demoras (seg)"; SL;SL;
    cout << "Tamanno lado\t" << "Bucle ij\t" << "Bucle ji\t"; SL;

    vector<float> lapsos_ij, lapsos_ji;
    lapsos_ij.resize(tamannos.size());
    lapsos_ji.resize(tamannos.size());

    cronometro reloj;


    reloj.est_funcion(matrint::operator*);
    transform(tamannos.begin(),tamannos.end(),lapsos_ij.begin(),reloj);

    reloj.est_funcion(matrint::producto_ji);
    transform(tamannos.begin(),tamannos.end(),lapsos_ji.begin(),reloj);

    tabulador(tamannos,lapsos_ij,lapsos_ji);

    SL;SL;
    /// ////////////////////////////////////////////*/

	return 0;
}
