#include <bits/stdc++.h>
using namespace std;

class Ativo {
public:
	
	void adicionaLimitBuy(int quantidade, double preco, int id) {
		ordensCompras.emplace(pair<double, int>(-preco, id), quantidade); // negativo pra maior encima
		resolveFilaVendas();
		resolveOrdens();
	}

	void adicionaLimitSell(int quantidade, double preco, int id) {
		ordensVendas.emplace(pair<double, int>(preco, id), quantidade);
		resolveFilaCompras();
		resolveOrdens();
	}

	void adicionaMarketBuy(int quantidade) {
		filaCompras.push(quantidade);
		resolveFilaCompras();
	}

	void adicionaMarketSell(int quantidade) {
		filaVendas.push(quantidade);
		resolveFilaVendas();
	}

	void imprimeLivro() {
		int W = 40;

		cout << "+" << string(W + 2, '-') << "+" << string(W + 2, '-') << "+\n";
		cout << "| " << left << setw(W) << "Ordens de Compra" << " | " << left << setw(W) << "Ordens de Venda" << " |\n"; 
		cout << "+" << string(W + 2, '-') << "+" << string(W + 2, '-') << "+\n";

		auto it_compra = ordensCompras.begin();
		auto it_venda = ordensVendas.begin();

		while (it_compra != ordensCompras.end() or it_venda != ordensVendas.end()) {
			cout << "| ";

			if (it_compra != ordensCompras.end()) {
				int quantidade = (*it_compra).second;
				double preco = -(*it_compra).first.first;

				stringstream ss;
				ss << quantidade << " @ " << fixed << setprecision(2) << preco;

				cout << left << setw(W) << ss.str();
				it_compra = next(it_compra);
			} else {
				cout << left << setw(W) << "";
			}

			cout << " | ";

			if (it_venda != ordensVendas.end()) {
				int quantidade = (*it_venda).second;
				double preco = (*it_venda).first.first;

				stringstream ss;
				ss << quantidade << " @ " << fixed << setprecision(2) << preco;

				cout << left << setw(W) << ss.str();

				it_venda = next(it_venda);
			} else {
				cout << left << setw(W) << "";
			}

			cout << " |\n";
		}
		cout << "+" << string(W + 2, '-') << "+" << string(W + 2, '-') << "+\n";
	}

private:
	queue<int> filaCompras, filaVendas;
	map<pair<double, int>, int> ordensCompras, ordensVendas;

	void notificaTrade(double preco, int quantidade) {
		cout << "Trade, price: " << preco << ", qty: " << quantidade << "\n";
	}

	void resolveOrdens() {
		while (not ordensVendas.empty() and not ordensCompras.empty()) {
			auto it_compra = ordensCompras.begin();
			auto it_venda = ordensVendas.begin();

			if (-(*it_compra).first.first < (*it_venda).first.first) break;

			int tira = min((*it_compra).second, (*it_venda).second);

			notificaTrade(-(*it_compra).first.first, tira);

			if (((*it_compra).second -= tira) == 0) { 
				ordensCompras.erase(it_compra);
			}
			if (((*it_venda).second -= tira) == 0) { 
				ordensVendas.erase(it_venda);
			}
		}
	}

	void resolveFilaCompras() {
		while (not filaCompras.empty()) {
			if (ordensVendas.empty()) break;
			
			int& quant = filaCompras.front();
			for (auto it = ordensVendas.begin(); it != ordensVendas.end() and quant > 0; ) {
				int tira = min(quant, (*it).second);

				notificaTrade((*it).first.first, tira);

				if (((*it).second -= tira) == 0) {
					it = ordensVendas.erase(it);
				}
				if ((quant -= tira) == 0) {
					break;
				}
			}

			if (quant == 0) {
				filaCompras.pop();
			}
		}
	}

	void resolveFilaVendas() {
		while (not filaVendas.empty()) {
			if (ordensCompras.empty()) break;
			
			int& quant = filaVendas.front();
			for (auto it = ordensCompras.begin(); it != ordensCompras.end() and quant > 0; ) {
				int tira = min(quant, (*it).second);

				notificaTrade(-(*it).first.first, tira);

				if (((*it).second -= tira) == 0) {
					it = ordensCompras.erase(it);
				}
				if ((quant -= tira) == 0) {
					break;
				}
			}

			if (quant == 0) {
				filaVendas.pop();
			}
		}
	}
};

int main() {
	int queries; cin >> queries;

	Ativo gestor;

	for (int id = 0; id < queries; id++) {
		string tipo, lado; cin >> tipo >> lado;

		if (tipo == "print" and lado == "book") {
			gestor.imprimeLivro();
		} else if (tipo == "limit") {
			double preco; cin >> preco;
			int quantidade; cin >> quantidade;

			cout << ">>> " << tipo << ' ' << lado << ' ' << preco << ' '<< quantidade << '\n';

			if (lado == "buy") {
				gestor.adicionaLimitBuy(quantidade, preco, id);
			} else if (lado == "sell") {
				gestor.adicionaLimitSell(quantidade, preco, id);
			} else assert(0);
		} else if (tipo == "market") {
			int quantidade; cin >> quantidade;

			cout << ">>> " << tipo << ' ' << lado << ' ' << quantidade << '\n';

			if (lado == "buy") {
				gestor.adicionaMarketBuy(quantidade);
			} else if (lado == "sell") {
				gestor.adicionaMarketSell(quantidade);
			} else assert(0);
		} else assert(0);
	}
	
	return(0);
}
