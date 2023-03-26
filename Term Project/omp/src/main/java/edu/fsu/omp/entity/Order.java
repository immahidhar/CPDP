package edu.fsu.omp.entity;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

import javax.persistence.*;

@Entity
@Getter
@Setter
@ToString
@Table(name="Order")
public class Order {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    private Integer id;
    @OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name="userId", referencedColumnName = "id", nullable=false)
    private User user;
    /*@Column
    private Integer userId;*/
    /*@OneToOne
    @JoinColumn(name="productId", nullable=false)
    private Product product;*/
    @Column
    private Integer productId;
    @Column
    private int quantity;
    @Column
    private String address;
    @Column
    private String status;
}
