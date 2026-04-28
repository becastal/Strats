#include <bits/stdc++.h>
using namespace std;

int main() {
	int queries; cin >> queries;

	queue<int> filaCompras, filaVendas;
	map<pair<double, int>, int> ordensCompras, ordensVendas; // (valor {ou -valor}, id), quantidade

	auto notificaTrade = [&](double price, int quant) {
		cout << "Trade, price: " << price << ", qty: " << quant << "\n";
	};

	auto resolveOrdens = [&]() {
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
	};

	auto resolveFilaCompras = [&]() {
		while (not filaCompras.empty()) {
			if (ordensVendas.empty()) break;
			
			int& quant = filaCompras.front();
			for (auto it = ordensVendas.begin(); it != ordensVendas.end(); ) {
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
	};

	auto resolveFilaVendas = [&]() {
		while (not filaVendas.empty()) {
			if (ordensCompras.empty()) break;
			
			int& quant = filaVendas.front();
			for (auto it = ordensCompras.begin(); it != ordensCompras.end(); ) {
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
	};

	for (int id = 0; id < queries; id++) {
		string tipo, lado; cin >> tipo >> lado;


		if (tipo == "limit") {
			double preco; cin >> preco;
			int quantidade; cin >> quantidade;

			cout << ">>> " << tipo << ' ' << lado << ' ' << preco << ' '<< quantidade << '\n';

			if (lado == "buy") {
				ordensCompras.emplace(pair<double, int>(-preco, id), quantidade); // negativo pra maior encima
				resolveFilaVendas();
				resolveOrdens();
			} else if (lado == "sell") {
				ordensVendas.emplace(pair<double, int>(preco, id), quantidade);
				resolveFilaCompras();
				resolveOrdens();
			} else assert(0);
		} else if (tipo == "market") {
			int quantidade; cin >> quantidade;

			cout << ">>> " << tipo << ' ' << lado << ' ' << quantidade << '\n';

			if (lado == "buy") {
				filaCompras.push(quantidade);
				resolveFilaCompras();
			} else if (lado == "sell") {
				filaVendas.push(quantidade);
				resolveFilaVendas();
			} else assert(0);
		} else assert(0);
	}
	
	return(0);
}
