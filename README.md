# data-struct-design
# 2.3 功能要求
>本设计要求精心设计问题中变元、文字、子句、公式等有效的物理存储结构，基于DPLL过程实现一个高效SAT求解器，对于给定的中小规模算例进行求解，输出求解结果，统计求解时间。要求具有如下功能：

+ **1.输入输出功能：包括程序执行参数的输入，SAT算例cnf文件的读取，执行结果的输出与文件保存等。(15%)**

+ **2.公式解析与验证：读取cnf算例文件，解析文件，基于一定的物理结构，建立公式的内部表示；并实现对解析正确性的验证功能，即遍历内部结构逐行输出与显示每个子句，与输入算例对比可人工判断解析功能的正确性。数据结构的设计可参考文献[1-3]。(15%)**

+ **3.DPLL过程：基于DPLL算法框架，实现SAT算例的求解。(35%)**

+ **4.时间性能的测量：基于相应的时间处理函数（参考time.h），记录DPLL过程执行时间（以毫秒为单位），并作为输出信息的一部分。(5%)**

+ **5.程序优化：对基本DPLL的实现进行存储结构、分支变元选取策略[1-3]等某一方面进行优化设计与实现，提供明确的性能优化率结果。优化率的计算公式为：[(t-to)/t] * 100%,其中t 为未对DPLL优化时求解基准算例的执行时间，to则为优化DPLL实现时求解同一算例的执行时间。(15%)**

功能（1）至（5）为基础功能，占功能分值的85%。
SAT应用：将二进制数独游戏[5,6]问题转化为SAT问题[6]，并集成到上面的求解器进行问题求解，游戏可玩，具有一定的/简单的交互性。应用问题归约为SAT问题的具体方法可参考文献[3]与[6-9]。（15%）

![图片1.png](https://i.loli.net/2020/01/11/PvM75XmkZxT3CYQ.png)

# 程序模块化
设计程序要求模块化，程序源代码进行模块化组织。主要模块包括如下：

+ 主控、交互与显示模块（display）
+ CNF解析模块（cnfparser）
+ 核心DPLL模块( solver)
+ 二进制数独模块,包括游戏格局生成、归约、求解(BinaryPuzzle)

# CNF公式的内部存储结构
  本应用处理的主要数据对象有变元或文字、子句、公式等。同学们可以分析这些数据的逻辑关系及其施加的基本运算而建立相应的抽象数据类型，设计其物理存储结构。如子句有创建createClause、销毁destroyClause、增加addClause、删除removeClause、判断是否为单子句isUnitClause、评估子句的真假状态evaluateClause等运算。由于每个CNF公式变元与子句数可能不同，同一个实例中子句长度也可能不等,一种基本的处理方式是将子句表示为由文字构成的链表；整个公式则是由子句构成的链表，如图2.5所示，这里仅供参考（也许并非最优结构），同学们可自行设计相应的物理存储结构并进行优化,有效支持回溯。
