project Teste

entity Professor {
	pk long codigo;
	text nome;
	unique text cpf;
}

entity Aluno {
	pk long codigo;
	text nome;
	unique integer matricula;
}

relationship alunos from Professor to Aluno 1..N