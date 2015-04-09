//����ʽ�������
#include <iostream>
#include<math.h>
#define NUM 15     //�ڵ�ĸ���
#define N 3           //��ϵĶ���ʽ�Ĵ���


void f1(double *,double *);           //����ʼx[i]��ֵ�ĸ��ݴη��洢��һ����ά�������� 
void f2(double *,double *,double [ ]);        //�������淽�����ϵ������ 
void DirectLU(double a[N][N+1],double [ ]);         //����ԪLU�ֽ�
void swap(double &,double &);          //��������������ֵ
using namespace std;

int test()
{
	int i,j;
	double x[NUM]; //�洢ԭʼ�Ľڵ�
	x[0]=1;
	for(i=1;i<NUM;i++)
		x[i]=x[i-1]+0.5;
	double y[NUM]={33.4,79.50,122.65,159.05,189.15,214.15,238.65,252.50,
		267.55,280.50,296.65,301.40,310.40,318.15,325.15};

	double a[N][NUM];
	double b[N][N+1]; //���淽�����ϵ��������Ҳ����
	double c[N];

	f1(a[0],x);
	cout<<"-------------------------"<<endl;
	for(i=0;i<N;i++)
	{
		for(j=0;j<NUM;j++)
			cout<<a[i][j]<<'\t';
		cout<<endl<<endl;
	}
	f2(a[0],b[0],y); //�������淽�����ϵ������
	cout<<"-------------------------"<<endl;
	cout<<"���淽�����ϵ������Ϊ: \n";
	for(i=0;i<N;i++)
	{
		for(j=0;j<=N;j++)
			cout<<b[i][j]<<'\t';
		cout<<endl;
	}

	DirectLU(b,c); //����ԪLU�ֽ�

	cout<<"-------------------------"<<endl;
	cout<<"��Ϻ�����ϵ���ֱ�Ϊ:\n";
	for(i=0;i<N;i++)
		cout<<"a["<<i<<"]="<<c[i]<<endl;
	cout<<"-------------------------"<<endl;
	return 0;
}

void f1(double *a,double *x)
{
	int i,j,k;
	double temp;
	for(i=0;i<N;i++)
		for(j=0;j<NUM;j++)
		{
			temp=1;
			for(k=0;k<i;k++)
				temp*=x[j];
			*(a+i*NUM+j)=temp;
		}
}

void f2(double *a,double *b,double y[])
{
	int i,j,k;
	double temp2;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			temp2=0;
			for(k=0;k<NUM;k++)
				temp2+=*(a+i*NUM+k)*(*(a+j*NUM+k));
			*(b+i*(N+1)+j)=temp2;
		}
		temp2=0;
		for(k=0;k<NUM;k++)
		{
			temp2+=y[k]*(*(a+i*NUM+k));
			*(b+i*(N+1)+N)=temp2;
		}
	}
}

void swap(double &a,double &b)
{
	a=a+b;
	b=a-b;
	a=a-b;
}

void DirectLU(double a[N][N+1],double x[])
{
	int i,r,k,j;
	double s[N],t[N];
	double max;
	for(r=0;r<N;r++)
	{
		max=0;
		j=r;
		for(i=r;i<N;i++) //����s[i]�ľ���ֵ,ѡ��Ԫ
		{
			s[i]=a[i][r];
			for(k=0;k<r;k++)
				s[i]-=a[i][k]*a[k][r];
			s[i]=s[i]>0?s[i]:-s[i]; //s[i]ȡ����ֵ
			if(s[i]>max){
				j=i;
				max=s[i];
			}
		}
		if(j!=r) //ѡ������Ԫ������j������r,���������ӦԪ�ؽ��е���
		{
			for(i=0;i<N+1;i++)
				swap(a[r][i],a[j][i]);
		}
		for(i=r;i<N+1;i++) //�����r�е�Ԫ��
			for(k=0;k<r;k++)
			{
				a[r][i]-=a[r][k]*a[k][i];
			}
			for(i=r+1;i<N;i++) //�����r�е�Ԫ��
			{
				for(k=0;k<r;k++)
					a[i][r]-=a[i][k]*a[k][r];
				a[i][r]/=a[r][r];
			}
	}
	for(i=0;i<N;i++)
		t[i]=a[i][N];
	for(i=N-1;i>=0;i--) //���ûش��������ս�
	{
		for(r=N-1;r>i;r--)
			t[i]-=a[i][r]*x[r];
		x[i]=t[i]/a[i][i];
	}
}