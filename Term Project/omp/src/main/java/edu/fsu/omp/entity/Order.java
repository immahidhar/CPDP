package edu.fsu.omp.entity;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

import javax.persistence.*;

@Entity
@Getter
@Setter
@ToString
@Table(name="Orders")
public class Order {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    private Integer id;
    /*@OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name="user_id", referencedColumnName = "id", nullable=false)
    private User user;*/
    @Column(name = "user_id")
    private Integer userId;
    /*@OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name="product_id", nullable=false)
    private Product product;*/
    @Column(name = "product_id")
    private Integer productId;
    @Column
    private Integer quantity;
    @Column
    private String address;
    @Column
    private String status;
}
