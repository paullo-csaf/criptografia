use std::io::{self, Write};

fn exponenciacao_modular(mut base: i64, mut exp: i64, mod_val: i64) -> i64 {
    let mut resultado: i64 = 1;
    base = base % mod_val;
    while exp > 0 {
        if exp % 2 == 1 {
            resultado = (resultado * base) % mod_val;
        }
        exp = exp / 2;
        base = (base * base) % mod_val;
    }
    resultado
}

fn main() {
    let p: i64 = 11;
    let q: i64 = 29;
    let n: i64 = p * q; 
    let e: i64 = 3;
    let d: i64 = 187;

    print!("Digite o texto: ");
    io::stdout().flush().unwrap();

    let mut m = String::new();
    io::stdin().read_line(&mut m).unwrap();
    let m = m.trim_end().to_string();

    // 1. Converte os caracteres para os códigos ASCII originais
    let numeros: Vec<i64> = m.chars().map(|c| c as i64).collect();

    // Exibição do código ANTES da encriptação
    println!("\n--- CÓDIGO ANTES DA ENCRIPTAÇÃO (ASCII) ---");
    for num in &numeros {
        print!("{} ", num);
    }
    println!();

    // 2. Criptografia
    println!("\n--- CÓDIGO DA MENSAGEM ENCRIPTADA ---");
    let mut criptografado: Vec<i64> = Vec::new();
    for &num in &numeros {
        let c = exponenciacao_modular(num, e, n);
        print!("{} ", c);
        criptografado.push(c);
    }
    println!();

    // 3. Descriptografia
    println!("\n--- CÓDIGO DA MENSAGEM DESENCRIPTADA ---");
    let mut descri: Vec<i64> = Vec::new();
    for &c in &criptografado {
        let de = exponenciacao_modular(c, d, n);
        print!("{} ", de);
        descri.push(de);
    }
    println!();

    // Reconstrói a string a partir dos números descriptografados
    let m_reconstruido: String = descri.iter()
        .map(|&num| num as u8 as char)
        .collect();

    println!("\n--- RESULTADOS FINAIS ---");
    println!("Texto original:         {}", m);
    println!("Texto desencriptado:    {}", m_reconstruido);
}