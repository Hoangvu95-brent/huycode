package UML;

interface Expression {
    int eval();
}
class VarExp implements Expression{
    private int var;
    public VarExp() {};
    public void setValue(int n) {
        var = n;
    }
    public int eval() {
        /*
         * AAAAAAAAAAAAAAAAAAAAAAAAAAAAA
         * */
        return this.var;
    }
}
class SeqExp implements Expression{
    private static final char[] dau = {'+', '-', '*', '/'};
    /*
    * BBBBBBBBBBBBBBBBBBBBBBB
    * */
    private int op;
    private Expression exp1;
    private Expression exp2;
    public SeqExp(Expression e1, Expression e2, int a_op) {
        exp1 = e1;
        exp2 = e2;
        op = a_op;
    }

    public int eval() {
        /*
        * Tinh gia tri bieu thuc
        * */
        switch (op) {
            case 0:
                return exp1.eval() + exp2.eval();
            case 1:
                return exp1.eval() - exp2.eval();
            case 2:
                return exp1.eval() * exp2.eval();
            case 3:
                return exp1.eval() / exp2.eval();
        }
        return 0;
    }
    public SeqExp operate(Expression e, int a_op) {
        /*
        * tao bieu thuc moi bang viec ket noi 2 bieu thuc da co
        * su dung cac toan tu + _ * /
        * CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        * */
        return new SeqExp(this, e, a_op);
    }
}
public class TestExpression {     
	public static void main(String args[]) {         
		VarExp a = new VarExp();         
		VarExp b = new VarExp();         
		SeqExp sum = new SeqExp(a, b, 0);         
		SeqExp diff = new SeqExp(a, b, 1);         
		SeqExp mul = sum.operate(diff,2);         
		a.setValue(3);         
		b.setValue(7);         
		System.out.print(mul.eval()); 
		}  
	} 