#include <bits/stdc++.h>
using namespace std;

class Ativo {
public:
	
	void adicionaLimitBuy(int id, int quantidade, double preco) {
		auto [it, ok] = ordensCompras.emplace(pair<double, int>(-preco, id), quantidade); // negativo pra maior encima
		indiceCompras[id] = it;

		resolveFilaVendas();
		resolveOrdens();
	}

	void adicionaLimitSell(int id, int quantidade, double preco) {
		auto [it, ok] = ordensVendas.emplace(pair<double, int>(preco, id), quantidade);
		indiceVendas[id] = it;

		resolveFilaCompras();
		resolveOrdens();
	}

	void adicionaMarketBuy(int id, int quantidade) {
		filaCompras.push(id);
		quantidadeFila[id] = quantidade;

		resolveFilaCompras();
	}

	void adicionaMarketSell(int id, int quantidade) {
		filaVendas.push(id);
		quantidadeFila[id] = quantidade;

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
				ss << quantidade << " @ " << preco;

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

	void cancelaOrdem(int id) {
		auto it_compra = indiceCompras.find(id);
		if (it_compra != indiceCompras.end()) {
			removeCompra((*it_conot mpra).second);
			return;
		}

		auto it_venda = indiceVendas.find(id);
		if (it_venda != indiceVendas.end()) {
			removeVenda((*it_venda).second);
			return;
		}

		if (quantidadeFila.count(id)) {
			quantidadeFila[id] = 0;
		}
	}

	void alteraOrdemMarket(int id, int novaQuantidade) {
		if (not quantidadeFila.count(id) or quantidadeFila[id] == 0) return;
		quantidadeFila[id] = novaQuantidade;	
	}

	void alteraOrdemLimit(int id, int novaQuantidade, double novoPreco) {
		bool ehCompra = indiceCompras.count(id), ehVenda = indiceVendas.count(id);
		if (not ehCompra and not ehVenda) return;

		cancelaOrdem(id);
		if (ehCompra) {
			adicionaLimitBuy(id, novaQuantidade, novoPreco);		
		} else {
			adicionaLimitSell(id, novaQuantidade, novoPreco);		
		}
	}

private:
	queue<int> filaCompras, filaVendas; // [id da ordem market]
	map<pair<double, int>, int> ordensCompras, ordensVendas; // [[preco, id], quantidade]
	map<int, map<pair<double, int>, int>::iterator> indiceCompras, indiceVendas; // [id, iterador pra ordens*]
	map<int, int> quantidadeFila; // [id, quantidade da ordem market]

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
				removeCompra(it_compra);
			}
			if (((*it_venda).second -= tira) == 0) { 
				removeVenda(it_venda);
			}
		}
	}

	void resolveFilaCompras() {
		while (not filaCompras.empty()) {
			if (ordensVendas.empty()) break;
			
			int id = filaCompras.front();
			int& quant = quantidadeFila[id];
			for (auto it = ordensVendas.begin(); it != ordensVendas.end() and quant > 0; ) {
				int tira = min(quant, (*it).second);

				notificaTrade((*it).first.first, tira);

				if (((*it).second -= tira) == 0) {
					it = removeVenda(it);
				}
				if ((quant -= tira) == 0) {
					break;
				}
			}

			if (quant == 0) {
				filaCompras.pop();
				quantidadeFila.erase(id);
			}
		}
	}

	void resolveFilaVendas() {
		while (not filaVendas.empty()) {
			if (ordensCompras.empty()) break;
			
			int id = filaVendas.front();
			int& quant = quantidadeFila[id];
			for (auto it = ordensCompras.begin(); it != ordensCompras.end() and quant > 0; ) {
				int tira = min(quant, (*it).second);

				notificaTrade(-(*it).first.first, tira);

				if (((*it).second -= tira) == 0) {
					it = removeCompra(it);
				}
				if ((quant -= tira) == 0) {
					break;
				}
			}

			if (quant == 0) {
				filaVendas.pop();
				quantidadeFila.erase(id);
			}
		}
	}

	map<pair<double, int>, int>::iterator removeCompra(map<pair<double, int>, int>::iterator it) {
		int id = (*it).first.second;
		indiceCompras.erase(id);
		return ordensCompras.erase(it);
	}

	map<pair<double, int>, int>::iterator removeVenda(map<pair<double, int>, int>::iterator it) {
		int id = (*it).first.second;
		indiceVendas.erase(id);
		return ordensVendas.erase(it);
	}
};

int main() {
	int queries; cin >> queries;

	Ativo gestor;

	for (int id = 0; id < queries; id++) {
		string tipo; cin >> tipo;

		if (tipo == "print") {
			gestor.imprimeLivro();
		} else if (tipo == "cancel") {
			int idOrdem; cin >> idOrdem;

			cout << ">>> cancel id: " << idOrdem << "\n";
			gestor.cancelaOrdem(idOrdem);

		} else if (tipo == "change") {
			string qual; cin >> qual;

			if (qual == "limit") {
				int idOrdem; cin >> idOrdem;
				double novoPreco; cin >> novoPreco;
				int novaQuantidade; cin >> novaQuantidade;

				cout << ">>> change limit id: " << idOrdem << ", new price: " << novoPreco << ", newQty: " << novaQuantidade << "\n";

				gestor.alteraOrdemLimit(idOrdem, novaQuantidade, novoPreco);
			} else if (qual == "market") {
				int idOrdem; cin >> idOrdem;
				int novaQuantidade; cin >> novaQuantidade;

				gestor.alteraOrdemMarket(idOrdem, novaQuantidade);
			}
		} else if (tipo == "limit") {
			string lado; cin >> lado;
			double preco; cin >> preco;
			int quantidade; cin >> quantidade;

			cout << ">>> new limit " << lado << " order price: " << preco << ", qty: "<< quantidade << '\n';

			if (lado == "buy") {
				gestor.adicionaLimitBuy(id, quantidade, preco);
			} else if (lado == "sell") {
				gestor.adicionaLimitSell(id, quantidade, preco);
			} else assert(0);
		} else if (tipo == "market") {
			string lado; cin >> lado;
			int quantidade; cin >> quantidade;

			cout << ">>> new market " << lado << " order qty: "<< quantidade << '\n';

			if (lado == "buy") {
				gestor.adicionaMarketBuy(id, quantidade);
			} else if (lado == "sell") {
				gestor.adicionaMarketSell(id, quantidade);
			} else assert(0);
		} else assert(0);
	}
	
	return(0);
}
