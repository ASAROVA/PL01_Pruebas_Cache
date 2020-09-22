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
    }
    //virtual ~matriz() { vvt::~vvt(); }

    matriz(int num_filas, int num_columnas, T val = T())
    {
        nfilas = num_filas;
        ncolum = num_columnas;
        this->resize(nfilas);
        for (auto& i: *this)
        {
            i.resize(ncolum);
            for(auto& ij: i) ij = val;
        }
    }

    matriz<T> operator* (const matriz<T>& M) const
    {
        if(M.nfilas!=ncolum)
            throw invalid_argument( "Dimensiones incompatibles" );

        matriz<T> P(nfilas,M.ncolum,T());

        for(int i=0; i<nfilas; i++)
        {
            for(int j=0; j<M.ncolum; j++)
                for(int k=0; k<ncolum; k++)
                    P[i][j] += (*this)[i][k]*M[k][j];
        }
    return P;
    }

    void rellenar ( int minimo=MIN, int maximo=MAX   )
    {
        for (auto& i: *this)
            for(auto& ij: i) ij = aleatorio(minimo,maximo);
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

/// Functor que mide la demora de una función ////
template<typename T, typename TF>
class cronometro
{
    TF f;
public:
    cronometro() {}
    void est_funcion(TF F) {f=F;}
    double operator()(int alto, int ancho=0)
    {
        if(!ancho) ancho = alto;

        matriz<T> M(alto,ancho);
        M.rellenar();
        matriz<T> N(alto,ancho);
        N.rellenar();

        clock_t t;
        t = clock();
        (M.*f)(N);
        t = clock()-t;
        return double(t)/double(CLOCKS_PER_SEC);
        //return double(t);
    }
};
/// ////////////////////////////////////////////*/


/// Tipo de funcion a medir y otras funciones ////
typedef int tipo;
typedef matriz<tipo>(matriz<tipo>::*funcion)(const matriz<tipo>&)const;

template <class T, class... TY>
void tabulador(vector<T> X, TY... Ys);
template <class T>
void interpolar(vector<T>& v,T inicio,T incremento,T fin);
/// ////////////////////////////////////////////*/


int main(void)
{
    cout << "\n\t\t >> ASAROVA << \n\n";

    funcion f;

    //vector<int> tamannos {100,200,400,800,1600};
    //vector<int> tamannos {100,200,400};
    vector<int> tamannos; interpolar(tamannos,100,100,1500);

    cout << "Tabla de Demoras (seg)"; SL;SL;
    cout << "Tamanno lado\t" << "Prod. Normal\t" << "-\t"; SL;

    vector<float> lapsos_ij, lapsos_ji;
    lapsos_ij.resize(tamannos.size());
    lapsos_ji.resize(tamannos.size());

    /// Usamdo el functor "cronometro" /////////////////

    cronometro<tipo,funcion> reloj;

    reloj.est_funcion(matriz<tipo>::operator*);
    transform(tamannos.begin(),tamannos.end(),lapsos_ij.begin(),reloj);

    //reloj.est_funcion(matriz<tipo>::producto_ji);
    //transform(tamannos.begin(),tamannos.end(),lapsos_ji.begin(),reloj);

    tabulador(tamannos,lapsos_ij,lapsos_ji);

    SL;SL;
    /// ////////////////////////////////////////////*/

    //cout << demora<tipo,funcion>(f,400); SL;SL;

    /*// Prueba M*N ///////////////////////////////////
    matrint M ( {{4,5,6},
                 {1,2,3},
                 {9,8,7},
                 {5,7,3}} );

    matrint N ( {{2,3,1,9},
                 {5,6,4,1},
                 {8,9,7,5}} );
    cout << M*N; SL;SL;

    /// ////////////////////////////////////////////*/

	return 0;
}

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


