project PaiDeArthur

entity Cliente {
	pk long id;
	text nome;
	unique text cpf;
	text endereco;
}

entity Produto {
	pk long id;
	text nome;
	long quantidade;
}

entity ProdutoCliente {
	real preco;
}

entity Venda {
	pk long id;
	date dataVenda;
}

relationship cliente from ProdutoCliente to Cliente N..1
relationship produto from ProdutoCliente to Produto N..1
relationship produtos from Venda to ProdutoCliente 1..N

