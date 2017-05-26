
project PapaiSabeTudo

entity Admin {
	pk long id;
	text login;
	text senha;
}

entity Escola {
	pk long id;
	text nome;
	text login;
	text senha;
}

entity Turma {
	pk long id;
	text identificador;
}

entity Funcionario {
	pk long id;
	text nome;
	text login;
	text senha;
}

entity PerfilAluno {
	pk long id;
	text nome;
	date aniversario;
	text login;
	text senha;
	text imageURL;
}

entity EventoComer {
	pk long id;
	text descricao;
	integer quantidadeGramas;
	time timestamp;
	boolean visualizado;
}

entity EventoBeber {
	pk long id;
	text descricao;
	integer quantidadeMl;
	time timestamp;
	boolean visualizado;
}

entity EventoBanho {
	pk long id;
	text observacao;
	time timestamp;
	boolean visualizado;
}

entity EventoCoco {
	pk long id;
	time timestamp;
	integer tipo;
	text observacao;
	boolean visualizado;
}

entity EventoEntrada {
	pk long id;
	time timestamp;
	text portador;
	boolean visualizado;
}

entity EventoSaida {
	pk long id;
	time timestamp;
	text portador;
	boolean visualizado;
}

weak relationship alunos from Escola to PerfilAluno 1..N 
weak relationship funcionarios from Escola to Funcionario 1..N
weak relationship evtSaida from PerfilAluno to EventoSaida 1..N
weak relationship evtEntrada from PerfilAluno to EventoEntrada 1..N
weak relationship evtCoco from PerfilAluno to EventoCoco 1..N
weak relationship evtBanho from PerfilAluno to EventoBanho 1..N
weak relationship evtBeber from PerfilAluno to EventoBeber 1..N
weak relationship evtComer from PerfilAluno to EventoComer 1..N
weak relationship alunos from Turma to PerfilAluno 1..N bilateral(turma)
weak relationship turmas from Escola to Turma 1..N

