package edu.fsu.omp.data;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class OrderDTO {
    private Integer id;
    private Integer userId;
    private Integer productId;
    private int quantity;
    private String address;
    private Status status;
}
