#include "math.hpp"

// Апроксимация умножения градиента функции f в точке х на вектор dx
// Погрешность O(||h||^2*||dx||^3), где h - выбранный шаг дифференцирования
// Авторы: Козырев Дмитрий, Бадави Полина
ld grad_prod_vect(Function f, const Vector& x, const Vector& dx) {
    if (is_zero(dx)) {
        return 0;
    }
    // Подбор шага h таким образом, чтобы погрешность составила 1e-8:
    ld h = std::sqrt(1e-8 / std::pow(norm(dx), 3.0));
	return (1 / (2*h)) * (f(x + h * dx) - f(x - h * dx));
}

// Апроксимация умножения матрицы Гессе в точке x на вектор dx 
// Погрешность O(||h||^2*||dx||^3)
// Авторы: Козырев Дмитрий, Бадави Полина
Vector hess_prod_vect(Function f, const Vector& x, const Vector& dx) {
    if (is_zero(dx)) {
        return Vector((int)x.size(), 0);
    }
    // Подбор шага h таким образом, чтобы погрешность составила 1e-8:
    ld h = std::sqrt(1e-8 / std::pow(norm(dx), 3.0));
	return (1 / (2*h)) * (grad(f, x+h*dx) - grad(f, x-h*dx));
}

// Авторы: Козырев Дмитрий, Бадави Полина
Vector conjugade_gradient(Matrix A, Vector b, Vector x) {
    auto d = - (A * x + b);
    for (int i = 0; i < (int)x.size() && !is_zero(d); ++i) {
        ld alpha = -(dot(d, A*x+b)) / dot(d, A * d);
        x += alpha * d;
        d = -(A * x + b) + (dot(A * x + b, A * d) / dot(d, A * d)) * d;    
    }
    return x;
}

// Авторы: Козырев Дмитрий, Бадави Полина
std::pair<Vector, int> slow_hessian_free(Function f, Vector x, int iter_limit) {
    int n = (int)x.size();
    int iterations = 0;
    for (iterations = 0; iterations < iter_limit; ++iterations) {
        auto dx = conjugade_gradient(hess(f, x), grad(f, x), Vector(n, 0));
        if (is_zero(dx) || std::abs(f(x) - f(x+dx)) < COMPARE_EPS) {
            break;
        }
        x += dx;
    }
    return {x, iterations};
}

// Авторы: Козырев Дмитрий, Бадави Полина
std::pair<Vector, int> hessian_free(Function f, Vector x, int iter_limit) {
    int n = (int)x.size();
    int iterations;
    for (iterations = 0; iterations < iter_limit; ++iterations) {
        Vector dx(n, 0);
        auto d = - (hess_prod_vect(f, x, dx) + grad(f, x));
        for (int j = 0; j < n && !is_zero(d); ++j) {
            ld alpha = -(dot(d, hess_prod_vect(f, x, dx) + grad(f, x)) / dot(d, hess_prod_vect(f, x, d)));
            dx += alpha * d;
            auto temp = hess_prod_vect(f, x, dx) + grad(f, x);
            d = -temp + dot(temp, hess_prod_vect(f, x, d)) / dot(d, hess_prod_vect(f, x, d)) * d;
        }
        if (is_zero(dx) || std::abs(f(x) - f(x+dx)) < COMPARE_EPS) {
            break;
        }
        x += dx;
    }
    return {x, iterations};
}