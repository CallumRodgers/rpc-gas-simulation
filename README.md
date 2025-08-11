# Análise de Mistura de Gases em RPCs

## Introdução:

Neste projeto, nosso objetivo é encontrar misturas sustentáveis para substituir
a mistura padrão utilizada nas RPCs do CERN. Utilizando Geant4 para a geometria
e como framework geral, e o Garfield++ para a simulação do campo elétrico e das
avalanches nos gases simulados, pretendemos encontrar misturas candidatas a possíveis
substituições nas RPCs reais, que serão submetidas a testes em laboratório.

## Grupo:

Instituição: Centro Brasileiro de Pesquisas Físicas\
Pesquisador: Gilvan Augusto Alves\
Alunos:
1. Callum Rodgers
2. Thiago Rangel
2. Vitor Ferretti

## Estrutura:

A geometria do detector é definida no Geant4 na classe `PMDetectorConstruction.cc/hh`. A interface entre Garfield++
e Geant4 (seguindo o PAI model segundo referência) é realizada de através da parametrização de um Physics 
Process no Geant4, na classe `PMHeedModel.cc/hh`, que é uma subclasse da `G4VFastSimulationModel`. A parametrização
de um processo físico requere três implementações:

1. Onde o processo deve ser executado (i.e. nossa região do gás da RPC, dada por uma `G4Region`);
2. Quais partículas devem ser simuladas no processo (até o momento, somente múons);
3. A implementação do processo físico propriamente dita: dinâmica da partícula naquele step, criação
de secundárias ou não, e energia depositada.

Os requerimentos 1 e 2 são feitos puramente no Geant4, na implementação dos métodos `ModelTrigger(G4FastTrack &)` e
`IsApplicable(G4ParticleDefinition &)`, respectivamente.

O requerimento 3, implementado em `DoIt(G4FastTrack &, G4FastStep &)`, é onde o Garfield++ é chamado para realizar a simulação
da partícula através do gás.

As três implementações são mais detalhadas a seguir:

### 1. Local da parametrização

Devemos fornecer ao Geant4 a região em que queremos que a nossa parametrização seja válida (a parametrização é
a simulação "customizada" de um processo físico, ou seja, de forma independente dos algoritmos próprios ao Geant4).
No nosso caso, a transição entre os softwares deve ocorrer nas regiões gasosas das RPCs. Em uma câmara com mais de um
gap, isto implica em múltiplos volumes acoplados a esta região.

Em "palavras de Geant4", isto significa que o nosso `PMHeedModel.cc/hh` será acoplado a uma única `G4Region`, que
por sua vez possui um (ou mais) `G4LogicalVolume` como raízes, que nada mais são que os mesmos `G4LogicalVolume`s que
correspondem aos volumes de gás.

Na nossa implementação, um objeto `G4Envelope` é declarado no header da classe `PMDetectorConstruction`. 
Cada geometria de RPC, inicializada no método `Construct()`, tem como responsabilidade inicializar este objeto. 

Após esta etapa, criamos, no método `ConstructSDandField()`, um objeto da nossa parametrização `PMHeedModel()`, cujo construtor leva como argumento justamente a `G4Region` (equivalente
à `G4Envelope`) criada anteriormente. A mera criação do objeto com `new PMHeedModel()` é suficiente, pois o Geant4 sendo
uma _state machine_ dá conta de registrar nosso modelo nos Managers adequados.

O método `ModelTrigger()` tem como argumento um objeto `G4FastTrack`, que nos fornece informações sobre uma partícula
sendo simulada ao usarmos `G4FastTrack.GetPrimaryTrack()`. Segundo a definição do método, devemos retornar uma booleana,
`G4bool`, que diga ao Geant4 se queremos que a partícula seja simulada com nosso processo customizado ou não.
É aqui que filtramos partículas dadas certas propriedades. Por exemplo, podemos retornar `false` se os múons
entrando na região possuem uma energia muito baixa. Podemos também realizar testes na posição da partícula,
em coordenadas locais ao envelope de gás.

### 2. Partículas a serem simuladas no Garfield++

A implementação do método `IsApplicable()` é extremamente simples. Como o nome implica, devemos retornar
novamente uma booleana dizendo se nossa parametrização é aplicável ou não, mas desta vez levando em conta o tipo
de partícula. Para aceitarmos somente múons, por exemplo, podemos implementar o método da seguinte forma:

```c++
G4bool PMHeedModel::IsApplicable(const G4ParticleDefinition& partDef) {
    if (&partDef == G4MuonPlus::Definition() ||
        &partDef == G4MuonMinus::Definition()) {
        return true;
    }
    return false;
}
```

Ou seja, basta comparar o argumento passado com a definição das partículas a serem aceitas.

Isto implica, também, que não precisamos fazer este teste no `ModelTrigger()`.

### 3. Simulação no Garfield++

A interface entre o Geant4 e o Garfield++ segue um algoritmo detalhado na referência [1].
Em suma: o Geant4 gera as partículas (e.g. múons) e **também** as transporta no volume gasoso até
que a energia cinética dela caia consideravelmente (para alguns keV), ou que ela saia do volume, gerando assim todos os
pares elétron-íons da ionização do gás devido à passagem da partícula. Neste momento,
o Garfield++ assume o controle e transporta estes elétrons com o `TrackHeed.TransportDeltaElectron()`.

Este modelo é _diferente_ do modelo que utiliza `TrackHeed.NewTrack()`. Ao invés de "matarmos" a partícula
no Geant4 assim que ela entra no volume gasoso, e passarmos as informações ao Garfield (por exemplo,
com o `TrackHeed.SetParticle()`), a geração das ionizações primárias no gás passa a ser responsabilidade do
Geant4, e o trabalho do Garfield se reduz ao transporte dos elétrons e a simulação da avalanche eletrônica.

Embora exija uma implementação mais complexa, ao usarmos este modelo de interface temos um conjunto de
benefícios. Primeiro, esta interface dá conta tanto de partículas com energias relativísticas (e.g. 
o feixe do LHC) quanto não-relativísticas, enquanto o modelo `TrackHeed.NewTrack()`,
segundo a referência [1], funciona corretamente apenas para partículas relativísticas. Em segundo lugar,
tanto o modelo PAI do Geant4 quanto o `TrackHeed.TransportDeltaElectron()` incluem o espalhamento de
Coulomb, enquanto o `TrackHeed.NewTrack()` não inclui, o que torna nosso método mais fisicamente preciso.

## Parâmetros Interativos

O Geant4 nos possibilita criar parâmetros customizados para serem definidos interativamente durante a execução
do programa (da mesma forma que podemos chamar e.g. /run/beamOn). 
Por exemplo, podemos escolher o gás sendo simulado ao passarmos um arquivo de gás (gerado pelo Magboltz) para o programa. 
A árvore de parâmetros segue, atualmente, a seguinte estrutura:

```
/rpc/
  +-- /setup/
  |      |-- /rpcType [type number]
  |      |-- /gasFile [filepath]
  |      |-- /voltage [value in Volts]
  +-- /beam/
         |-- /energy [value in eV]
```

## Referências

[1] "Interfacing Geant4, Garfield++ and Degrad for the simulation of gaseous
detectors", (https://doi.org/10.1016/j.nima.2019.04.110)
