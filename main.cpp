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

	void adicionaPegBidBuy(int id, int quantidade) {
		ordensPegCompra[id] = quantidade;

		resolveFilaVendas();
		resolveOrdens();
	}

	void adicionaPegOfferSell(int id, int quantidade) {
		ordensPegVenda[id] = quantidade;

		resolveFilaCompras();
		resolveOrdens();
	}

	void imprimeLivro() {
		int W = 40;
		auto it_compra = ordensCompras.begin();
		auto it_venda = ordensVendas.begin();
		auto it_peg_compra = ordensPegCompra.begin();
		auto it_peg_venda = ordensPegVenda.begin();
		bool temPegCompra = not ordensCompras.empty();
		bool temPegVenda = not ordensVendas.empty();
		double precoPegCompra = (temPegCompra ? -(*ordensCompras.begin()).first.first : 0);
		double precoPegVenda = (temPegVenda ? (*ordensVendas.begin()).first.first : 0);

		cout << "+" << string(W + 2, '-') << "+" << string(W + 2, '-') << "+\n";
		cout << "| " << left << setw(W) << "Ordens de Compra" << " | " << left << setw(W) << "Ordens de Venda" << " |\n"; 
		cout << "+" << string(W + 2, '-') << "+" << string(W + 2, '-') << "+\n";

		while (it_compra != ordensCompras.end() or it_venda != ordensVendas.end() or
			   (temPegCompra and it_peg_compra != ordensPegCompra.end()) or
			   (temPegVenda and it_peg_venda != ordensPegVenda.end())) {
			cout << "| ";

			if (it_compra != ordensCompras.end() or (temPegCompra and it_peg_compra != ordensPegCompra.end())) {
				bool usaPeg = false;

				if (temPegCompra and it_peg_compra != ordensPegCompra.end()) {
					if (it_compra == ordensCompras.end()) {
						usaPeg = true;
					} else {
						double preco = -(*it_compra).first.first;
						int id = (*it_compra).first.second;
						usaPeg = (preco < precoPegCompra or (preco == precoPegCompra and (*it_peg_compra).first < id));
					}
				}

				int quantidade;
				double preco;
				if (usaPeg) {
					quantidade = (*it_peg_compra).second;
					preco = precoPegCompra;
					it_peg_compra = next(it_peg_compra);
				} else {
					quantidade = (*it_compra).second;
					preco = -(*it_compra).first.first;
					it_compra = next(it_compra);
				}

				stringstream ss;
				ss << quantidade << " @ " << preco;
				if (usaPeg) ss << " (pegged)";

				cout << left << setw(W) << ss.str();
			} else {
				cout << left << setw(W) << "";
			}

			cout << " | ";

			if (it_venda != ordensVendas.end() or (temPegVenda and it_peg_venda != ordensPegVenda.end())) {
				bool usaPeg = false;

				if (temPegVenda and it_peg_venda != ordensPegVenda.end()) {
					if (it_venda == ordensVendas.end()) {
						usaPeg = true;
					} else {
						double preco = (*it_venda).first.first;
						int id = (*it_venda).first.second;
						usaPeg = (preco > precoPegVenda or (preco == precoPegVenda and (*it_peg_venda).first < id));
					}
				}

				int quantidade;
				double preco;
				if (usaPeg) {
					quantidade = (*it_peg_venda).second;
					preco = precoPegVenda;
					it_peg_venda = next(it_peg_venda);
				} else {
					quantidade = (*it_venda).second;
					preco = (*it_venda).first.first;
					it_venda = next(it_venda);
				}

				stringstream ss;
				ss << quantidade << " @ " << fixed << setprecision(2) << preco;
				if (usaPeg) ss << " (pegged)";

				cout << left << setw(W) << ss.str();
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
			removeCompra((*it_compra).second);
			return;
		}

		auto it_venda = indiceVendas.find(id);
		if (it_venda != indiceVendas.end()) {
			removeVenda((*it_venda).second);
			return;
		}

		if (quantidadeFila.count(id)) {
			quantidadeFila[id] = 0;
			return;
		}

		if (ordensPegCompra.count(id)) ordensPegCompra.erase(id);
		if (ordensPegVenda.count(id)) ordensPegVenda.erase(id);
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
	map<int, int> ordensPegCompra, ordensPegVenda; // [id, quantidade]

	void notificaTrade(double preco, int quantidade) {
		cout << "Trade, price: " << preco << ", qty: " << quantidade << "\n";
	}

	void resolveOrdens() {
		while (not ordensVendas.empty() and not ordensCompras.empty()) {
			auto it_compra = ordensCompras.begin();
			auto it_venda = ordensVendas.begin();
			auto it_peg_compra = ordensPegCompra.begin();
			auto it_peg_venda = ordensPegVenda.begin();

			double precoCompra = -(*it_compra).first.first;
			double precoVenda = (*it_venda).first.first;

			if (precoCompra < precoVenda) break;

			bool usaPegCompra = (it_peg_compra != ordensPegCompra.end() and (*it_peg_compra).first < (*it_compra).first.second);
			bool usaPegVenda = (it_peg_venda != ordensPegVenda.end() and (*it_peg_venda).first < (*it_venda).first.second);

			int quantidadeCompra = (usaPegCompra ? (*it_peg_compra).second : (*it_compra).second);
			int quantidadeVenda = (usaPegVenda ? (*it_peg_venda).second : (*it_venda).second);
			int tira = min(quantidadeCompra, quantidadeVenda);

			notificaTrade(precoCompra, tira);

			if (usaPegCompra) {
				if (((*it_peg_compra).second -= tira) == 0) {
					ordensPegCompra.erase(it_peg_compra);
				}
			} else if (((*it_compra).second -= tira) == 0) { 
				removeCompra(it_compra);
			}

			if (usaPegVenda) {
				if (((*it_peg_venda).second -= tira) == 0) {
					ordensPegVenda.erase(it_peg_venda);
				}
			} else if (((*it_venda).second -= tira) == 0) { 
				removeVenda(it_venda);
			}
		}
	}

	void resolveFilaCompras() {
		while (not filaCompras.empty()) {
			if (ordensVendas.empty()) break;
			
			int id = filaCompras.front();
			int& quant = quantidadeFila[id];
			while (not ordensVendas.empty() and quant > 0) {
				auto it_venda = ordensVendas.begin();
				auto it_peg_venda = ordensPegVenda.begin();
				bool usaPegVenda = (it_peg_venda != ordensPegVenda.end() and (*it_peg_venda).first < (*it_venda).first.second);
				int quantidadeVenda = (usaPegVenda ? (*it_peg_venda).second : (*it_venda).second);
				int tira = min(quant, quantidadeVenda);

				notificaTrade((*it_venda).first.first, tira);

				if (usaPegVenda) {
					if (((*it_peg_venda).second -= tira) == 0) {
						ordensPegVenda.erase(it_peg_venda);
					}
				} else if (((*it_venda).second -= tira) == 0) {
					removeVenda(it_venda);
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
			while (not ordensCompras.empty() and quant > 0) {
				auto it_compra = ordensCompras.begin();
				auto it_peg_compra = ordensPegCompra.begin();
				bool usaPegCompra = (it_peg_compra != ordensPegCompra.end() and (*it_peg_compra).first < (*it_compra).first.second);
				int quantidadeCompra = (usaPegCompra ? (*it_peg_compra).second : (*it_compra).second);
				int tira = min(quant, quantidadeCompra);

				notificaTrade(-(*it_compra).first.first, tira);

				if (usaPegCompra) {
					if (((*it_peg_compra).second -= tira) == 0) {
						ordensPegCompra.erase(it_peg_compra);
					}
				} else if (((*it_compra).second -= tira) == 0) {
					removeCompra(it_compra);
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

			if (lado == "bid" or lado == "offer") {
				cin >> lado;
			}

			double preco; cin >> preco;
			int quantidade; cin >> quantidade;

			cout << ">>> new limit " << lado << " order price: " << preco << ", qty: "<< quantidade << '\n';

			if (lado == "buy") {
				gestor.adicionaLimitBuy(id, quantidade, preco);
			} else if (lado == "sell") {
				gestor.adicionaLimitSell(id, quantidade, preco);
			} else assert(0);
		} else if (tipo == "peg") {
			string referencia, lado; cin >> referencia >> lado;
			int quantidade; cin >> quantidade;

			cout << ">>> new peg " << referencia << " " << lado << " order qty: " << quantidade << '\n';

			if (referencia == "offer" and lado == "buy") {
				gestor.adicionaMarketBuy(id, quantidade);
			} else if (referencia == "bid" and lado == "sell") {
				gestor.adicionaMarketSell(id, quantidade);
			} else if (referencia == "bid" and lado == "buy") {
				gestor.adicionaPegBidBuy(id, quantidade);
			} else if (referencia == "offer" and lado == "sell") {
				gestor.adicionaPegOfferSell(id, quantidade);
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
