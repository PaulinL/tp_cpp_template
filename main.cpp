/*

L'objectif est d'écrire une hiérarchie de classe de matrice: matrice dense, triangulaire sup, diagonale.

1. Écrire une classe matrix_t_ abstraite avec comme interface
	- operator(i, j) virtuelle pure
	- trace() qui renvoit la somme des éléments sur la diagonale
	- print() qui affiche la matrice dans le terminale
	=> trace() et print() sont à implémenter dans matrix_t_

2. Écrire une classe matrix_dense qui hérite de matrice_t_ et implémente operator(i,j)

3. Écrire une classe matrix_triangulaire_sup qui hérite de matrice_t_ et implémente operator(i,j)

4. Écrire une classe matrix_diagonal qui hérite de matrice_t_ et implémente operator(i,j)

5. En utilisant std::chrono, effetuer des mesures de performances de l'implémentation de trace() et print() 
dans le cas des matrices diagonales et triangulaires pour des tailles de matrices croissantes. Commenter les résultats
obtenus.

6. Rendre trace() et print() virtuelle dans matrix_t_ et les ré-implémenter pour les classes filles

7. Discuter si operator()(i,j) a du sens pour "écrire" dans une matrice non dense ?
 operator()(i,j) a du sens pour une matrice dense car on veut pouvoir acceder a chacune des donnes du tableau. En
 revanche, il n'est pas tres coherent pour une matrice diagonale. En effet, seul les elements de la diagonale sont
 interessants et nous pouvons les recuperer facilement avec seulement l'index dans la diagonale. Cet operator est
 cependant plus interessant pour une matrice triangulaire car on doit pouvoir acceder a la moitie des elements de la
 matrice (ici la moitie haute).

8. Implémenter une fonction virtuelle add qui prend deux matrices de types quelconques et qui renvoit une matrice
du bon type contenant la somme des éléments des deux matrices. Comment résoudre la problème de connaissance des types 
des deux matrices d'entrées pour en déduire le type de sortie ? Quel outil du standard pouvez vous utiliser pour renvoyer
un objet polymorphe depuis cette fonction membre ?

*/
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <memory>
#include <numeric>

template<typename T>
class matrix_dense;

template<typename T>
class matrix_triangulaire_sup;

template<typename T>
class matrix_diag;

template<typename T>
class matrix_t_ {
protected:
    std::size_t height;
    std::size_t width;
    std::vector<T> data;

public:
    matrix_t_(int height, int width) : height(height), width(width) {}

    virtual T &operator()(std::size_t const &, std::size_t const &) = 0;

    virtual const T &operator()(std::size_t const &, std::size_t const &) const = 0;

    virtual void print() {
        for (std::size_t i = 0; i < height; i++) {
            for (std::size_t j = 0; j < width; j++)
                std::cout << (*this)(i, j) << "\t";
            std::cout << std::endl;
        }
    }

    virtual T trace() {
        T sum = {};
        for (std::size_t i = 0; i < std::min(height, width); i++) {
            sum += (*this)(i, i);
        }
        return sum;
    }

    virtual std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m1, const matrix_t_<T> &m2) = 0;

    virtual std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m) const = 0;

    virtual std::unique_ptr<matrix_t_<T>> add(const matrix_dense<T> &m) const = 0;

    virtual std::unique_ptr<matrix_t_<T>> add(const matrix_triangulaire_sup<T> &m) const = 0;

    virtual std::unique_ptr<matrix_t_<T>> add(const matrix_diag<T> &m) const = 0;

};


template<typename T>
class matrix_dense : public matrix_t_<T> {

public:
    matrix_dense(int height, int width) : matrix_t_<T>(height, width) {
        this->data = std::vector<T>(height * width);
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width)
            return this->data[row + (col * this->height)];
        else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width)
            return this->data[row + (col * this->height)];
        else
            throw std::out_of_range("Out of range.");
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m1, const matrix_t_<T> &m2) override {
        return m1.add(m2);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m) const override {
        return m.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_dense<T> &m1) const override {
        matrix_dense<T> result(m1);
        for (std::size_t i = 0; i < m1.height; i++)
            for (std::size_t j = 0; j < m1.width; j++)
                result(i, j) += (*this)(i, j);
        return std::make_unique<matrix_dense<T>>(result);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_triangulaire_sup<T> &m1) const override {
        matrix_dense<T> result(this->height, this->width);
        for (std::size_t i = 0; i < this->height; i++)
            for (std::size_t j = 0; j < this->width; j++)
                result(i, j) = (*this)(i, j) + (m1)(i, j);
        return std::make_unique<matrix_dense<T>>(result);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_diag<T> &m1) const override {
        matrix_dense<T> result(this->height, this->width);
        for (std::size_t i = 0; i < result.height; i++)
            for (std::size_t j = 0; j < this->width; j++)
                result(i, j) = (*this)(i, j) + (m1)(i, j);
        return std::make_unique<matrix_dense<T>>(result);
    }
};

template<typename T>
class matrix_triangulaire_sup : public matrix_t_<T> {
private:
    T valInf;

public:
    matrix_triangulaire_sup(int height, int width, T valInf) : matrix_t_<T>(height, width), valInf(valInf) {
        std::size_t size;
        if (height >= width)
            size = (width * (width + 1) / 2);
        else
            size = (width * (width + 1) / 2) - (std::abs(height - width) * ((std::abs(height - width) + 1)) / 2);
        this->data = std::vector<T>(size);
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width) {
            if (row > col)
                return valInf;
            else {
                return this->data[col + row * this->width - (row * (row + 1)) / 2];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width) {
            if (row > col)
                return valInf;
            else {
                return this->data[col + row * this->width - (row * (row + 1)) / 2];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    T trace() override {
        T sum = {};
        for (std::size_t i = 0, j = 0; i < std::min(this->height, this->width); j += this->width - i, i++) {
            sum += this->data[j];
        }
        return sum;
    }

    void print() override {
        for (std::size_t i = 0; i < this->height; i++) {
            for (std::size_t j = 0; j < this->width; j++) {
                if (i <= j)
                    std::cout << (*this)(i, j) << "\t";
                else
                    std::cout << valInf << "\t";
            }
            std::cout << std::endl;
        }
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m1, const matrix_t_<T> &m2) override {
        return m1.add(m2);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m) const override {
        return m.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_dense<T> &m1) const override {
        return m1.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_triangulaire_sup<T> &m1) const override {
        matrix_triangulaire_sup<T> result(m1);
        for (std::size_t i = 0; i < m1.height; i++) {
            for (std::size_t j = i; j < m1.width; j++)
                result(i, j) += (*this)(i, j);
        }
        result.valInf += this->valInf;
        return std::make_unique<matrix_triangulaire_sup<T>>(result);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_diag<T> &m1) const override {
        matrix_triangulaire_sup<T> result(this->height, this->width, m1.getDefaultVal());
        for (std::size_t i = 0; i < this->height; i++) {
            for (std::size_t j = i; j < this->width; j++)
                result(i, j) = m1(i, j) + (*this)(i, j);
        }
        result.valInf += m1.getDefaultVal();
        return std::make_unique<matrix_triangulaire_sup<T>>(result);
    }
};

template<typename T>
class matrix_diag : public matrix_t_<T> {
private:
    T defaultVal;

public:
    matrix_diag(int height, int width, T defaultVal) : matrix_t_<T>(height, width), defaultVal(defaultVal) {
        this->data = std::vector<T>(std::min(height, width));
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width) {
            if (row != col)
                return defaultVal;
            else {
                return this->data[row];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width) {
            if (row != col)
                return defaultVal;
            else {
                return this->data[row];
            }
        } else
            throw std::out_of_range("Out of range.");
    }


    T trace() override {
        return std::accumulate(this->data.begin(), this->data.end(), 0);
    }


    void print() override {
        for (std::size_t i = 0; i < this->height; i++) {
            for (std::size_t j = 0; j < this->width; j++) {
                if (i == j)
                    std::cout << (*this)(i, j) << "\t";
                else
                    std::cout << defaultVal << "\t";
            }
            std::cout << std::endl;
        }
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m1, const matrix_t_<T> &m2) override {
        return m1.add(m2);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_t_<T> &m) const override {
        return m.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_dense<T> &m1) const override {
        return m1.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_triangulaire_sup<T> &m1) const override {
        return m1.add(*this);
    }

    std::unique_ptr<matrix_t_<T>> add(const matrix_diag<T> &m1) const override {
        matrix_diag<T> result(m1);
        for (std::size_t i = 0; i < this->data.size(); i++)
            result.data[i] += this->data[i];
        result.defaultVal += this->defaultVal;
        return std::make_unique<matrix_diag<T>>(result);
    }

    T getDefaultVal() const {
        return defaultVal;
    }
};

void testPerformance() {
    matrix_dense<int> m_dense = {20000, 20000};
    matrix_triangulaire_sup<int> m_triang = {20000, 20000, 0};
    matrix_diag<int> m_diag = {20000, 20000, 0};

    auto start = std::chrono::steady_clock::now();
    auto trace = m_dense.trace();
    auto end = std::chrono::steady_clock::now();

    std::cout << "Trace matrix dense : " << trace << std::endl
              << "Calculated in " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " µs" << std::endl;

    start = std::chrono::steady_clock::now();
    trace = m_triang.trace();
    end = std::chrono::steady_clock::now();

    std::cout << "Trace matrix triang sup : " << trace << std::endl
              << "Calculated in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " µs" << std::endl;

    start = std::chrono::steady_clock::now();
    trace = m_diag.trace();
    end = std::chrono::steady_clock::now();

    std::cout << "Trace matrix diagonal : " << trace << std::endl
              << "Calculated in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " µs" << std::endl;
}


int main() {


    // testPerformance();

    matrix_dense<int> m_dense(0, 0);
    matrix_diag<int> m_dense1(4, 4, 4);
    matrix_diag<int> m_dense2(4, 4, 1);

    m_dense1(1, 1) = 3;
    m_dense1(1, 2) = 3;

    m_dense2(1, 1) = 4;
    m_dense2(1, 3) = 4;

    m_dense1.print();
    std::cout << std::endl;
    m_dense2.print();
    std::cout << std::endl;
    auto r = m_dense.add(m_dense1, m_dense2);
    r->print();


    return 0;
}
