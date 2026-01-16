#!/usr/bin/env python3
"""
Aplicação para Gestão de Tratamentos Aqualin
E-fólio B 2525/2526 - Introdução à Programação
"""

import sys
from typing import List, Tuple, Optional


class Camara:
    """Representa uma câmara térmica"""
    def __init__(self, nome: str):
        self.nome = nome
        self.tratamentos = []  # Lista de (aqualin, saude_entrada, instante_entrada, instante_saida)
    
    def __str__(self):
        return self.nome


class Aqualin:
    """Representa um Aqualin no sistema"""
    def __init__(self, nome: str, saude: int, instante: int):
        self.nome = nome
        self.saude_entrada = saude
        self.saude_atual = saude
        self.instante_entrada = instante
        self.espera = 0
        self.instante_saida = None
        self.estado = None  # 'alta' ou 'morte'
        self.camara = None
    
    def __str__(self):
        return self.nome


def validar_nome(nome: str) -> bool:
    """Valida se o nome tem dois nomes unidos por traço"""
    partes = nome.split('-')
    return len(partes) == 2 and all(parte.strip() for parte in partes)


def calcular_tempo_tratamento(saude: int) -> int:
    """Calcula o tempo necessário para levar saúde de 'saude' até 100"""
    if saude >= 100:
        return 0
    
    tics = 0
    saude_atual = saude
    
    while saude_atual < 100:
        if saude_atual > 50:
            # +1 ponto por cada tic
            tics += (100 - saude_atual)
            saude_atual = 100
        elif saude_atual > 20:
            # +1 ponto por cada 10 tics
            # Vamos até 51 ou até 100
            pontos_ate_51 = 51 - saude_atual
            tics += pontos_ate_51 * 10
            saude_atual = 51
        elif saude_atual > 10:
            # +1 ponto por cada sol (100 tics)
            # Vamos até 21
            pontos_ate_21 = 21 - saude_atual
            tics += pontos_ate_21 * 100
            saude_atual = 21
        else:  # saude_atual > 0
            # +1 ponto por cada 10 sols (1000 tics)
            # Vamos até 11
            pontos_ate_11 = 11 - saude_atual
            tics += pontos_ate_11 * 1000
            saude_atual = 11
    
    return tics


def calcular_degradacao(saude_inicial: int, tempo_espera: int) -> int:
    """Calcula a saúde após 'tempo_espera' tics de espera"""
    saude = saude_inicial
    tics_restantes = tempo_espera
    
    while tics_restantes > 0 and saude > 0:
        if saude > 50:
            # -1 ponto por cada 10 sols (1000 tics)
            degradacoes = min(tics_restantes // 1000, saude - 50)
            if degradacoes > 0:
                saude -= degradacoes
                tics_restantes -= degradacoes * 1000
            else:
                if tics_restantes >= 1000:
                    saude -= 1
                    tics_restantes -= 1000
                else:
                    break
        elif saude > 20:
            # -1 ponto por cada sol (100 tics)
            degradacoes = min(tics_restantes // 100, saude - 20)
            if degradacoes > 0:
                saude -= degradacoes
                tics_restantes -= degradacoes * 100
            else:
                if tics_restantes >= 100:
                    saude -= 1
                    tics_restantes -= 100
                else:
                    break
        elif saude > 10:
            # -1 ponto por cada 10 tics
            degradacoes = min(tics_restantes // 10, saude - 10)
            if degradacoes > 0:
                saude -= degradacoes
                tics_restantes -= degradacoes * 10
            else:
                if tics_restantes >= 10:
                    saude -= 1
                    tics_restantes -= 10
                else:
                    break
        else:  # saude > 0
            # -1 ponto por cada tic
            degradacoes = min(tics_restantes, saude)
            saude -= degradacoes
            tics_restantes -= degradacoes
    
    return max(0, saude)


def main():
    camaras = []
    aqualins = []
    ultimo_instante = 0
    tratamentos_executados = False
    
    for linha in sys.stdin:
        linha = linha.strip()
        
        # Verificar se começa com #
        if not linha.startswith('#'):
            break
        
        # Remover o # e dividir em palavras
        partes = linha[1:].strip().split()
        
        # Verificar se tem pelo menos 1 palavra
        if len(partes) == 0:
            break
        
        # Verificar se tem no máximo 3 palavras
        if len(partes) > 3:
            continue
        
        comando = partes[0].lower()
        
        # PARTE 1: Adicionar câmara
        if len(partes) == 1 and comando != 'tratamentos':
            nome_camara = partes[0]
            if validar_nome(nome_camara):
                # Verificar se já existe
                if not any(c.nome == nome_camara for c in camaras):
                    camaras.append(Camara(nome_camara))
        
        # Relatório global
        elif comando == 'relatorio' and len(partes) == 2 and partes[1].lower() == 'global':
            print(f"Aqualins: {len(aqualins)}")
            print(f"Camaras: {len(camaras)}")
        
        # PARTE 2: Adicionar Aqualin
        elif len(partes) == 3 and comando not in ['relatorio', 'indicador']:
            nome_aqualin = partes[0]
            try:
                saude = int(partes[1])
                instante = int(partes[2])
                
                # Validar nome
                if not validar_nome(nome_aqualin):
                    continue
                
                # Validar instante
                if instante < ultimo_instante:
                    continue
                
                ultimo_instante = instante
                
                # Validar saúde
                if saude <= 0 or saude >= 100:
                    continue
                
                # Verificar se já existe
                if any(a.nome == nome_aqualin for a in aqualins):
                    continue
                
                # Adicionar Aqualin
                aqualins.append(Aqualin(nome_aqualin, saude, instante))
            except ValueError:
                continue
        
        # PARTE 3: Tratamentos
        elif comando == 'tratamentos' and len(partes) == 1:
            if len(camaras) == 0 or len(aqualins) == 0:
                continue
            
            # Resetar tratamentos anteriores
            for camara in camaras:
                camara.tratamentos = []
            
            # Copiar lista de aqualins para processamento
            fila_espera = aqualins.copy()
            
            # Estado das câmaras: (camara, instante_disponivel)
            estado_camaras = [(camara, 0) for camara in camaras]
            
            # Lista de eventos (instante, aqualin, tipo)
            altas = []
            mortes = []
            
            while fila_espera:
                # Próximo paciente
                aqualin = fila_espera.pop(0)
                
                # Encontrar câmara disponível mais cedo
                estado_camaras.sort(key=lambda x: (x[1], camaras.index(x[0])))
                camara, instante_disponivel = estado_camaras[0]
                
                # Calcular tempo de espera
                tempo_espera = max(0, instante_disponivel - aqualin.instante_entrada)
                aqualin.espera = tempo_espera
                
                # Calcular saúde após espera
                saude_apos_espera = calcular_degradacao(aqualin.saude_entrada, tempo_espera)
                aqualin.saude_atual = saude_apos_espera
                
                instante_inicio_tratamento = max(aqualin.instante_entrada, instante_disponivel)
                
                if saude_apos_espera <= 0:
                    # Morreu
                    aqualin.estado = 'morte'
                    aqualin.instante_saida = instante_inicio_tratamento
                    mortes.append(aqualin)
                else:
                    # Tratamento
                    tempo_tratamento = calcular_tempo_tratamento(saude_apos_espera)
                    instante_alta = instante_inicio_tratamento + tempo_tratamento
                    
                    aqualin.estado = 'alta'
                    aqualin.instante_saida = instante_alta
                    aqualin.camara = camara
                    altas.append(aqualin)
                    
                    # Registrar na câmara
                    camara.tratamentos.append((aqualin.nome, aqualin.saude_entrada, 
                                              instante_inicio_tratamento, instante_alta))
                    
                    # Atualizar estado da câmara
                    estado_camaras[0] = (camara, instante_alta)
            
            tratamentos_executados = True
            
            # Imprimir relatório de altas
            print("Altas:")
            if altas:
                # Ordenar por instante de alta, depois por ordem de entrada
                altas_ordenadas = sorted(altas, key=lambda a: (a.instante_saida, aqualins.index(a)))
                primeira = altas_ordenadas[0]
                ultima = altas_ordenadas[-1]
                
                print(f"- primeira: {primeira.nome} {primeira.saude_entrada} {primeira.instante_entrada} "
                      f"{primeira.espera} {primeira.instante_saida}")
                print(f"- ultima: {ultima.nome} {ultima.saude_entrada} {ultima.instante_entrada} "
                      f"{ultima.espera} {ultima.instante_saida}")
            
            if mortes:
                menor_saude = min(m.saude_entrada for m in mortes)
                maior_saude = max(m.saude_entrada for m in mortes)
                print(f"Mortes: {len(mortes)} {menor_saude} {maior_saude}")
        
        # PARTE 4: Relatórios
        elif comando == 'relatorio' and len(partes) == 2:
            tipo = partes[1].lower()
            
            if tipo == 'aqualins':
                for aqualin in aqualins:
                    if aqualin.estado:
                        estado_str = f"{aqualin.estado}:{aqualin.instante_saida}"
                        print(f"{aqualin.nome} {aqualin.saude_entrada} {aqualin.instante_entrada} "
                              f"{aqualin.espera} {estado_str}")
                    else:
                        print(f"{aqualin.nome} {aqualin.saude_entrada} {aqualin.instante_entrada} 0")
            
            elif tipo == 'camaras':
                for camara in camaras:
                    print(f"{camara.nome}:")
                    for nome, saude, entrada, saida in camara.tratamentos:
                        print(f"- {nome} {saude} {entrada} {saida}")
        
        # PARTE 4: Indicadores
        elif comando == 'indicador' and len(partes) == 2:
            tipo = partes[1].lower()
            
            if tipo == 'espera':
                aqualins_tratados = [a for a in aqualins if a.estado]
                if aqualins_tratados:
                    tempo_medio = sum(a.espera for a in aqualins_tratados) // len(aqualins_tratados)
                    print(tempo_medio)
                else:
                    print(0)
            
            elif tipo == 'trabalho':
                aqualins_tratados = [a for a in aqualins if a.estado]
                if aqualins_tratados:
                    total = sum(a.instante_saida - a.instante_entrada for a in aqualins_tratados)
                    print(total)
                else:
                    print(0)
            
            elif tipo == 'salvos':
                salvos = sum(1 for a in aqualins if a.estado == 'alta')
                print(salvos)


if __name__ == '__main__':
    main()
