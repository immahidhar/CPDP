package edu.fsu.omp.service;

import edu.fsu.omp.data.Category;
import edu.fsu.omp.data.ProductDTO;
import edu.fsu.omp.entity.Product;
import edu.fsu.omp.repo.ProductRepository;
import edu.fsu.omp.util.ModelMapperUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Component
public class ProductSearchService {
    @Autowired
    private ModelMapperUtil modelMapper;
    @Autowired
    private ProductRepository productRepository;
    public List<ProductDTO> search(String query) {
        List<ProductDTO> searchResult = new ArrayList<>();
        Iterable<Product> productInventory = productRepository.findAll();
        // search with product category
        for (Category category : Category.values()) {
            if(category.name().equalsIgnoreCase(query)) {
                List<Product> products = productRepository.findByCategory(query.toLowerCase(Locale.ROOT));
                searchResult.addAll(products.stream().map(this::convertToDTO).collect(Collectors.toList()));
            }
        }
        // search with product name
        for (Product product : productInventory) {
            if (product.getName().equalsIgnoreCase(query))
                searchResult.add(modelMapper.map(product, ProductDTO.class));
        }
        return searchResult;
    }
    private ProductDTO convertToDTO(Product product) {return modelMapper.map(product, ProductDTO.class);}
}
